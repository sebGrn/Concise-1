#include <string>
#include <map>
#include <utility>
#include "Runner.hpp"
#include<algorithm>
#include<functional>
#include<string>
#include<utility>
#include<vector>
#include <thread>
#include "ThreadPool.hpp"

Runner::Runner(const std::string& _outputFilename, float _threshold = 1., bool _useMeasures = false, bool _useMandatory = false, bool _useParallelism = false, bool _constantMemoryMode = false, bool _benchmarkMode = false) {
    this->threshold = _threshold;
    this->useMeasures = _useMeasures;
    this->nb_covered_tuples = 0;
    this->total = 0;

    this->measures = map<string, float>();
    this->measures["object uniformity"] = 0.;
    this->measures["monocle"] = 0.;
    this->measures["frequency"] = 0.;
    this->measures["separation"] = 0.;

    if (this->threshold > 1 || this->threshold < 0) {
        cout << "Threshold should be between 0 and 1, the algorithm will use 1 by default" << endl;
        this->threshold = 1.;
    }

    this->outputFilename = _outputFilename;
    this->dimensionSeparator = " ";

    this->useMandatory = _useMandatory;
    this->useParallelism = _useParallelism;
    this->constantMemoryMode = _constantMemoryMode;
    this->benchmarkMode = _benchmarkMode;
    this->nb_threads = std::thread::hardware_concurrency();

}

Runner::~Runner()
{
}

void Runner::prepareFile(const std::string& inputFilename)
{
    TransactionalDataParser parser = TransactionalDataParser();
    parser.parse(this, inputFilename);
    parser.initIntentConcept(this);
    this->dataStringToInt = parser.getMapStringToInt();
    this->dataIntToString = parser.getMapIntToString();

}

void Runner::runFcaCemb(const std::string& inputFilename)
{
    this->prepareFile(inputFilename);
    // compute everything
    this->computeFcaCemb();
}

string Runner::calculateMeasures(std::bitset<$_LINE_NUMBER>& extent, std::bitset<$_COLUMN_NUMBER>& intent)
{
    float objectUniformity = calculateMeasureObjectUniformity(extent, intent);
    this->measures["object uniformity"] += objectUniformity;

    float monocle = calculateMeasureMonocle(extent, intent);
    this->measures["monocle"] += monocle;

    float frequency = calculateMeasureFrequency(extent, intent);
    this->measures["frequency"] += frequency;

    float separation = calculateMeasureSeparation(extent, intent);
    this->measures["separation"] += separation;

    std::stringstream measure_stream;
    measure_stream << std::fixed << std::setprecision(5)
                   << "Object Uniformity=" << objectUniformity
                   << "; Monocle=" << monocle
                   << "; Frequency=" << frequency
                   << "; Separation=" << separation;

    return measure_stream.str();
}

float Runner::calculateMeasureObjectUniformity(std::bitset<$_LINE_NUMBER>& extent, std::bitset<$_COLUMN_NUMBER>& intent)
{
    float sum = 0;
    size_t a = extent.count();
    float axb = a * intent.count();
    for (size_t o = 0; o < extent.size(); o++) {
        if (!extent[o])
            continue;
        assert(o < this->concept.support.size());
        sum += axb / (a * this->concept.support[o]);
    }
    return sum / extent.count();
}

float Runner::calculateMeasureMonocle(std::bitset<$_LINE_NUMBER>& extent, std::bitset<$_COLUMN_NUMBER>& intent)
{
    float sumA = 0;
    float sumB = 0;

    for (size_t o = 0; o < extent.size(); o++) {
        if (!extent[o])
            continue;

        std::bitset<$_LINE_NUMBER> bitset;
        bitset[o] = 1;

        for (auto& extent_intent : this->mandatoryConcepts) {
            if ((bitset & (~extent_intent.first)).any())
                sumA++;
        }
        for (auto& extent_intent : this->nonMandatoryConcepts) {
            if ((bitset & (~extent_intent.first)).any())
                sumA++;
        }
    }

    for (size_t i = 0; i < intent.size(); i++) {
        if (!intent[i])
            continue;

        std::bitset<$_COLUMN_NUMBER> bitset;
        bitset[i] = 1;

        for (auto& extent_intent : this->mandatoryConcepts) {
            if ((bitset & (~extent_intent.second)).any())
                sumB++;
        }
        for (auto& extent_intent : this->nonMandatoryConcepts) {
            if ((bitset & (~extent_intent.second)).any())
                sumB++;
        }
    }

    float a = extent.count() + sumA;
    float b = intent.count() + sumB;
    return a * b;
}

float Runner::calculateMeasureFrequency(std::bitset<$_LINE_NUMBER>& extent, std::bitset<$_COLUMN_NUMBER>& intent)
{
    return static_cast<float>(extent.count()) / extent.size();
}

float Runner::calculateMeasureSeparation(std::bitset<$_LINE_NUMBER>& extent, std::bitset<$_COLUMN_NUMBER>& intent)
{
    float axb = extent.count() * intent.count();
    float sumg = 0;
    float sumb = 0;
    for (size_t o = 0; o < extent.size(); o++) {
        if (!extent[o])
            continue;
        assert(o < this->concept.extentNotReversed.size());
        sumg += this->concept.extentNotReversed[o].count();
    }
    for (size_t i = 0; i < intent.size(); i++) {
        if (!intent[i])
            continue;
        assert(i < this->concept.extent.size());
        sumb += this->concept.extent[i].count();
    }
    return axb / (sumg + sumb - axb);
}


void Runner::writeTuples()
{
    std::ofstream outputFile(this->outputFilename + ".out", ofstream::out);

    for (auto& extent_intent : this->mandatoryConcepts) {
        std::stringstream concept_stream;
        std::bitset<$_LINE_NUMBER> extent = extent_intent.first;
        std::bitset<$_COLUMN_NUMBER> intent = extent_intent.second;
        concept_stream << printExtent(extent) << " ; " << printIntent(intent) << " Mandatory";
        if (this->useMeasures)
            concept_stream << "#" << this->calculateMeasures(extent, intent);

        outputFile << concept_stream.str() << endl;
    }


    for (auto& extent_intent : this->nonMandatoryConcepts) {
        std::stringstream concept_stream;
        std::bitset<$_LINE_NUMBER> extent = extent_intent.first;
        std::bitset<$_COLUMN_NUMBER> intent = extent_intent.second;
        concept_stream << printExtent(extent) << " ; " << printIntent(intent);
        if (this->useMeasures)
            concept_stream << "#" << this->calculateMeasures(extent, intent);

        outputFile << concept_stream.str() << endl;
    }

    this->writeSummary();

    if (this->useMeasures) {
        Logger::start_tick();
        for (auto& measure : this->measures) {
            outputFile << "# Mean " << measure.first << ": " << std::fixed << std::setprecision(5) << (measure.second/ (this->nbMandatoryConcept + this->nbNonMandatoryConcept)) << endl;
            cout << "Mean " << measure.first << ": " << std::fixed << std::setprecision(5) << (measure.second/ (this->nbMandatoryConcept + this->nbNonMandatoryConcept)) << endl;
        }

        Logger::end_tick("Calculating measures");
    }

    outputFile.close();
}

void Runner::writeStats(){
    size_t totalSize = this->nbNonMandatoryConcept + this->nbMandatoryConcept;

    std::ofstream outputFileRessourceUsage(this->outputFilename + ".stats", ofstream::app);
    outputFileRessourceUsage << "Total: " << totalSize << endl;
    outputFileRessourceUsage << "Mandatory: " << this->nbMandatoryConcept << endl;
    outputFileRessourceUsage << "Non-mandatory: " << this->nbNonMandatoryConcept << endl;
    outputFileRessourceUsage << "Coverage: " << this->nb_covered_tuples << "/" << this->total << "(" << std::fixed << std::setprecision(5) << (static_cast<float>(100*this->nb_covered_tuples)/this->total) << "%)" << endl;

    outputFileRessourceUsage.close();
}


std::string Runner::printIntent(std::bitset<$_COLUMN_NUMBER> db)
{
    std::stringstream stream;
    bool firstItem = true;
    for (size_t i = 0; i < db.size(); i++) {
        if (db[i]) {
            if (firstItem) {
                stream << this->dataIntToString[i+1];
                firstItem = false;
            } else {
                stream << "," << this->dataIntToString[i+1];
            }
        }
    }
    return stream.str();
}

std::string Runner::printExtent(std::bitset<$_LINE_NUMBER> db)
{
    std::stringstream stream;
    bool firstItem = true;
    for (size_t i = 0; i < db.size(); i++) {
        if (db[i]) {
            if (firstItem) {
                stream << (i + 1);
                firstItem = false;
            } else {
                stream << "," << (i + 1);
            }
        }
    }
    return stream.str();
}


int Runner::lookForMandatoryFcaCemb(size_t i, vector<std::bitset<$_LINE_NUMBER>>& covered_tuples)
{
    int _nb_covered_tuples = 0;

    // get element
    assert(i < this->concept.extent.size());
    assert(i < this->concept.intent.size());
    std::bitset<$_LINE_NUMBER> c1_extent_i = this->concept.extent[i];
    std::bitset<$_COLUMN_NUMBER> c1_intent_i = this->concept.intent[i];

    bool valid = false;
    if (c1_extent_i.count() == 1)
        valid = true;

    size_t j = 0;

    while (!valid && j < c1_extent_i.size()) {
        if (c1_extent_i[j]) {
            assert(j < this->concept.support.size());
            size_t supp_o = this->concept.support[j];
            if (supp_o == c1_extent_i.count())
                valid = true;
        }
        j++;
    }

    if (valid) {

        for (size_t i1 = 0; i1 < c1_extent_i.size(); i1++) {

            assert(i1 < c1_extent_i.size());
            if (c1_extent_i[i1]) {

                for (size_t o1 = 0; o1 < c1_intent_i.size(); o1++) {

                    assert(o1 < c1_intent_i.size());
                    if (c1_intent_i[o1]) {

                        assert(o1 < covered_tuples.size());
                        assert(i1 < covered_tuples[o1].size());
                        if (!(covered_tuples[o1][i1])) {
                            covered_tuples[o1][i1] = true;
                            _nb_covered_tuples++;
                        }
                    }
                }
            }
        }

        pair<std::bitset<$_LINE_NUMBER>, std::bitset<$_COLUMN_NUMBER>> temp_pair = make_pair(c1_extent_i, c1_intent_i);

        if(_nb_covered_tuples > 0) {
            if(!this->benchmarkMode) {
                if (this->constantMemoryMode) {
                    if (this->nbNonMandatoryConcept == 0 and this->nbMandatoryConcept == 0)
                        this->writeSingleTuple(false, true, temp_pair);
                    else
                        this->writeSingleTuple(true, true, temp_pair);
                } else
                    this->mandatoryConcepts.push_back(temp_pair);
            }
            this->nbMandatoryConcept++;
        }
    }

    return _nb_covered_tuples;
}

void Runner::computeFcaCemb()
{
    this->nb_covered_tuples = 0;
    this->total = 0;
    vector<std::bitset<$_LINE_NUMBER>> covered_tuples($_COLUMN_NUMBER);


    for (size_t i = 0; i < $_COLUMN_NUMBER; i++)
        this->total += this->concept.extent[i].count();

    if (this->useMandatory) {
        Logger::start_tick();
        {
            // look for lookForMandatory
            for (size_t i = 0; i < $_COLUMN_NUMBER; i++) {
                this->nb_covered_tuples += this->lookForMandatoryFcaCemb(i, covered_tuples);
                if (this->nb_covered_tuples >= this->total*this->threshold)
                    break;
            }
        }
        Logger::end_tick("Mandatory");
    }

    if (this->nb_covered_tuples < this->total * this->threshold) {
        Logger::start_tick();
        // Generate list of indices to check
        vector<pair<size_t, size_t>> max_values_and_indices = vector<pair<size_t, size_t>>();
        for (size_t i = 0; i < $_COLUMN_NUMBER; i++) {
            pair<size_t, size_t> temp_pair = make_pair(this->concept.extent[i].count() * this->concept.intent[i].count(), i);
            max_values_and_indices.push_back(temp_pair);
        }
        // Sort descending order
        sort(max_values_and_indices.begin(), max_values_and_indices.end(), greater <pair<size_t, size_t>>());

        ThreadPool pool{nb_threads};

        auto getBestIndice = [this] (std::vector<pair<size_t, size_t>> curent_max_values_and_indices, vector<std::bitset<$_LINE_NUMBER>> covered_tuples){
            int max_val = -1;
            int best_i = -1;

            // Found the best for among each extent
            for (auto max_value_indice : curent_max_values_and_indices) {
                size_t i = max_value_indice.second;

                // Calculate number of not covered tuples
                int current_val = 0;
                assert(i < this->concept.intent.size());
                for (size_t o = 0; o < this->concept.intent[i].size();
                o++) {
                    // Test valid column
                    if (!this->concept.intent[i][o])
                    continue;

                    assert(o < this->concept.extent.size());
                    assert(o < covered_tuples.size());
                    /* on prend les 1 communs à l'attribut i et à l'attribut o quand i est compris dans o
                     * et on compte ceux qui ne sont pas encore couverts
                     * */
                    int val = (this->
                    concept.extent[i] & this->
                    concept.extent[o] & (~covered_tuples[o])).count();
                    current_val += val;
                }

                if (current_val > max_val) {
                    best_i = i;
                    max_val = current_val;
                }
            }

            return vector<int>({best_i, max_val});
        };

        while (this->nb_covered_tuples < this->total * this->threshold) {


            // look for best
            int max_val = -1;
            int best_i = -1;

            if(!this->useParallelism) {
                best_i = getBestIndice(max_values_and_indices, covered_tuples)[0];
            }else {

                size_t size = max_values_and_indices.size() / nb_threads;
                if(size <= 1){
                    auto result = getBestIndice(max_values_and_indices, covered_tuples);
                    best_i = result[0];
                }else {

                    vector<std::future<vector<int>>> currentProcesses = vector<std::future<vector<int>>>();
                    for (int i = 0; i < nb_threads; ++i) {

                        auto start_itr = std::next(max_values_and_indices.cbegin(), i*size);
                        auto end_itr = std::next(max_values_and_indices.cbegin(), i*size + size);

                        if (i == nb_threads - 1)
                            end_itr = max_values_and_indices.cend();

                        auto curent_max_values_and_indices = std::vector<pair<size_t, size_t>>(start_itr, end_itr);

                        currentProcesses.push_back(pool.enqueue([covered_tuples, this, curent_max_values_and_indices] {
                            int max_val = -1;
                            int best_i = -1;

                            // Found the best for among each extent
                            for (auto max_value_indice : curent_max_values_and_indices) {
                                size_t i = max_value_indice.second;

                                // Calculate number of not covered tuples
                                int current_val = 0;
                                assert(i < this->concept.intent.size());
                                for (size_t o = 0; o < this->concept.intent[i].size();
                                o++) {
                                    // Test valid column
                                    if (!this->concept.intent[i][o])
                                    continue;

                                    assert(o < this->concept.extent.size());
                                    assert(o < covered_tuples.size());
                                    /* on prend les 1 communs à l'attribut i et à l'attribut o quand i est compris dans o
                                     * et on compte ceux qui ne sont pas encore couverts
                                     * */
                                    int val = (this->
                                    concept.extent[i] & this->
                                    concept.extent[o] & (~covered_tuples[o])).count();
                                    current_val += val;
                                }

                                if (current_val > max_val) {
                                    best_i = i;
                                    max_val = current_val;
                                }
                            }

                            return vector<int>({best_i, max_val});
                        }));

                    }

                    max_val = -1;
                    best_i = -1;

                    for (int i = 0; i <nb_threads; ++i) {
                        auto result = currentProcesses.at(i).get();
                        if(result[1] > max_val){
                            max_val = result[1];
                            best_i = result[0];
                        }
                    }

                }
            }

            // Update covered tuples
            for (size_t o1 = 0; o1 < $_COLUMN_NUMBER; o1++) {
                if (!this->concept.intent[best_i][o1])
                    continue;
                for (size_t i1 = 0; i1 < $_LINE_NUMBER; i1++) {
                    if (!this->concept.extent[best_i][i1])
                    continue;

                    assert(o1 < covered_tuples.size());
                    assert(i1 < covered_tuples[o1].size());
                    if (!(covered_tuples[o1][i1])) {
                        covered_tuples[o1][i1] = true;
                        this->nb_covered_tuples++;
                    }
                }
            }

            pair<std::bitset<$_LINE_NUMBER>, std::bitset<$_COLUMN_NUMBER>> temp_pair = make_pair(this->concept.extent[best_i], this->concept.intent[best_i]);

            if(!this->benchmarkMode) {
                if (this->constantMemoryMode) {
                    if (this->nbNonMandatoryConcept == 0 and this->nbMandatoryConcept == 0)
                        this->writeSingleTuple(false, false, temp_pair);
                    else
                        this->writeSingleTuple(true, false, temp_pair);
                } else {
                    this->nonMandatoryConcepts.push_back(temp_pair);
                }
            }

            this->nbNonMandatoryConcept++;

            // Remove column from element to check
            for (auto it = max_values_and_indices.begin(); it != max_values_and_indices.end(); ++it) {
                if (best_i == it->second) {
                    max_values_and_indices.erase(it);
                    break;
                }
            }
        }
        Logger::end_tick("Non mandatory");
    }

    if(!this->benchmarkMode) {
        if (!this->constantMemoryMode)
            this->writeTuples();
        else {
            this->writeSummary();
        }
    }
}

void Runner::writeSingleTuple(bool append, bool mandatory, pair<std::bitset<$_LINE_NUMBER>, std::bitset<$_COLUMN_NUMBER>> tuple){
    std::ofstream outputFile;
    if(append)
        outputFile.open(this->outputFilename + ".out", ofstream::app);
    else
        outputFile.open(this->outputFilename + ".out", ofstream::out);

    std::stringstream concept_stream;
    std::bitset<$_LINE_NUMBER> extent = tuple.first;
    std::bitset<$_COLUMN_NUMBER> intent = tuple.second;
    if(mandatory)
        concept_stream << printExtent(extent) << " ; " << printIntent(intent) << " Mandatory";
    else
        concept_stream << printExtent(extent) << " ; " << printIntent(intent);
    if (this->useMeasures)
        concept_stream << "#" << this->calculateMeasures(extent, intent);

    outputFile << concept_stream.str() << endl;
    outputFile.close();
}

void Runner::writeSummary(){
    std::ofstream outputFile(this->outputFilename + ".out", ofstream::app);

    outputFile << "# Mandatory: " << this->nbMandatoryConcept << endl;
    outputFile << "# Non-mandatory: " << this->nbNonMandatoryConcept << endl;
    outputFile << "# Total: " << this->mandatoryConcepts.size() + this->nonMandatoryConcepts.size() << endl;
    outputFile << "# Coverage: " << this->nb_covered_tuples << "/" << this->total << "(" << std::fixed
               << std::setprecision(5) << (static_cast<float>(100 * this->nb_covered_tuples) / this->total) << "%)"
               << endl;
    outputFile << "# Number lines: " << $_LINE_NUMBER << endl;
    outputFile << "# Number columns: " << $_COLUMN_NUMBER << endl;
    outputFile.close();
}


