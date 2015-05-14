#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wdeprecated"


#include <iostream>
#include <map>
#include <vector>
#include <boost/program_options.hpp>
#include <random>
#include <algorithm/em_algorithm_thread_mutation.h>
#include <algorithm/em_algorithm_mutation.h>

#include "api/BamReader.h"
#include "evolution_models/F81.h"
#include "io_data/boost_input_utils.h"
#include "io_data/pileup_utils.h"
#include "algorithm/em_algorithm_mutation.h"
#include "algorithm/em_algorithm_mutation_v1.h"
#include "mutations/sequencing_factory.h"



#include "sys/types.h"
#include "sys/sysinfo.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
//#include "em_algorithm.h"
#include "mutations/mutation_model_multi_categories.h"

#include "setup_utils.h"

using namespace std;
using namespace BamTools;
/*
 *
 * Custom filter, remove sample 47 51 44   ->  7 8 10
M47_ATTGAGGA_L007	8
M44_GCCAAT_L001	7
M51_GGAGAACA_L007	10

 M50_CAGATC_L001	9
 M50_CAGATC_L005	9
 M531_CTTGTA_L001	11
 M531_CTTGTA_L005	11
 M40_ACAGTG_L005	6
 M40_ACAGTG_L001	6
 M44_GCCAAT_L005	7

 M29_CAGATCTG_L007	5
 M28_TGACCA_L001	4
 M28_TGACCA_L005	4

 M25_GTGTTCTA_L007	3
 M20_AAACATCG_L007	2
 M19_AACGTGAT_L007	1
 M0_CGATGT_L001	0
 M0_CGATGT_L005	0



 */


void CreateMutationModel(MutationModel &model, GenomeData &genome_data, ModelParams &params);

//GenomeData getGenomeData(boost::program_options::variables_map variables_map);

MutationModelMultiCategories CreateMutationModelMulti(GenomeData genome_data, ModelParams params);

void RunEmWithRealDataMultiThread(boost::program_options::variables_map map, size_t thread_count);


//void PostFilterGenomeData(GenomeData &genome_data);

void CustomFilterGenomeData(GenomeData &genome_data) {

//    Custom filter, remove sample 47 51 44   ->  7 8 10

    for (int g = genome_data.size()-1 ; g >= 0; --g) {

        ModelInput &data = genome_data[g];
//        PrintModelInput(data);
//        std::cout << "\t" << data.all_reads.size() << "\t" << data.all_reads.capacity() << "\t";
//        data.all_reads.erase(data.all_reads.begin()+10);
//        data.all_reads.erase(data.all_reads.begin()+7, data.all_reads.begin()+9);
//        data.all_reads.shrink_to_fit();


        std::swap(data.all_reads[10], data.all_reads.back());
        data.all_reads.pop_back();
        std::swap(data.all_reads[8], data.all_reads.back());
        data.all_reads.pop_back();
        std::swap(data.all_reads[7], data.all_reads.back());
        data.all_reads.pop_back();
//        std::cout << "\t" << data.all_reads.size() << "\t" << data.all_reads.capacity() << "\t" << std::endl;


        int sum = 0;
        for (int j = 0; j < 4; ++j) {
            sum += data.all_reads[0].reads[j];
        }

        if(sum < 6){//Bad ref, remove site
//            PrintModelInput(data);
//            genome_data.erase(genome_data.begin()+g);
            std::swap(genome_data[g], genome_data.back());
            genome_data.pop_back();
        }
//        else if(sum>150){
//            PrintReads(data.all_reads[0]);
//        }
        else {//Check each descendant
//            std::cout <<g << "\t" << data.all_reads.capacity() << "\t" << data.all_reads.size() << std::endl;
            for (int i = data.all_reads.size()-1 ; i > 0; --i) {
                int sum = 0;
                for (int j = 0; j < 4; ++j) {
                    sum += data.all_reads[i].reads[j];
                }
                if (sum < 6) {
//                    std::cout << g << "\t" << sum << "\t" << i << "\t" << data.all_reads.size() << "\t" << data.all_reads[i].key <<"\t" << data.all_reads[i-1].key <<"\t" << data.all_reads[i+1].key <<"\t";
//                    data.all_reads.erase(data.all_reads.begin()+i);//FIXME: current model assume equal des, fail if do this
                    std::swap(data.all_reads[i], data.all_reads.back());
                    data.all_reads.pop_back();

//                    if (i != pList.size() - 1)
//                    {
//                        // Beware of move assignment to self
//                        // see http://stackoverflow.com/questions/13127455/
//                        pList[i] = std::move(pList.back());
//                    }
//                    pList.pop_back();
                }
//                else if(sum>200){
//                    PrintReads(data.all_reads[i]);
//                }
            }

            if (data.all_reads.size() ==1) {
//                std::cout << g << "\t" << data.all_reads.size() << std::endl;
//                for (int i = 0; i < 9; ++i) {
//                    PrintReads(data.all_reads[i]);
//                }

                std::swap(genome_data[g], genome_data.back());
                genome_data.pop_back();

            }
        }

    }

}



void RefitData(boost::program_options::variables_map variables_map) {

    printMemoryUsage("Start ");
    ModelParams params = BoostUtils::CreateModelParams(variables_map);
    MutationProb mutation_prob (params);
    F81 evo_model0(mutation_prob);
    MutationModel mutation_model (evo_model0);

    printMemoryUsage("Start Basic");

    clock_t t1;
    t1 = clock();

    GenomeData genome_data = getGenomeData(variables_map);
    SummariseRealData(genome_data);
    CustomFilterGenomeData(genome_data);//TODO: Add this back
    SummariseRealData(genome_data);

//    std::vector<SiteGenotypesIndex> sgi;
//    SequencingFactory sequencing_factory(params);
//    sequencing_factory.CreateSequenceProbsVector(genome_data);



    cout << "Time: read genome data: " << ((clock() - t1) / CLOCKS_PER_SEC) << "\t" << (clock() - t1) << endl;
    cout << "===== Setup EmData. Init site_count: " << genome_data.size() << endl;
    printMemoryUsage("Read genomeData");

    std::exit(200);

    t1 = clock();
//    CreateMutationModel(mutation_model, genome_data, params);

    cout << "Time Create Mutation Model: " << ((clock() - t1) / CLOCKS_PER_SEC) << "\t" << (clock() - t1) << endl;
    cout << "===== Done preprocess. Final site count: " << mutation_model.GetSiteCount() << endl;
    printMemoryUsage("Created Mutation Model");


    cout << "===== Setup EM" << endl;
//    std::vector<std::unique_ptr<EmModel>> em_model2;
//    em_model2.emplace_back(new EmModelMutation(mutation_model));
//    em_model2.emplace_back(new EmModelMutation(mutation_model));
    const string &outfile_prefix = variables_map["outfile"].as<string>();

    clock_t t_start, t_end;
    t_start = clock();

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

//    EmAlgorithmMutation em_alg0(em_model2);
//    em_alg0.SetOutfilePrefix(outfile_prefix);
//    printMemoryUsage("EM");
//    em_alg0.Run();
//    em_alg0.PrintSummary();
//    std::string summary = em_alg0.GetEMSummary();
//    std::cout << summary << std::endl;

    t_end = clock();
    cout << "EM Clock time: " << (t_end - t_start) / CLOCKS_PER_SEC << "\t" << (t_end - t_start) << endl << endl;

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "EM started at "<< std::ctime(&start_time) << "EM finished at " << std::ctime(&end_time)  << "EM elapsed time: " << elapsed_seconds.count() << "s\n";

    printMemoryUsage("End EM");

}

//
//void RunEmWithRealDataMultiThread(boost::program_options::variables_map variables_map, size_t thread_count) {
//
//    printMemoryUsage("Start ");
//    ModelParams params = BoostUtils::CreateModelParams(variables_map);
//    MutationProb mutation_prob (params);
//    F81 evo_model0(mutation_prob);
////    MutationModel mutation_model (evo_model0);
//
//    printMemoryUsage("Start Basic");
//
//    clock_t t1;
//    t1 = clock();
//
//    GenomeData genome_data = getGenomeData(variables_map);
//    PostFilterGenomeData(genome_data);//TODO: Add this back
//    printMemoryUsage("Read genomeData");
//
//    cout << "Time: read genome data: " << ((clock() - t1) / CLOCKS_PER_SEC) << "\t" << (clock() - t1) << endl;
//    cout << "===== Setup EmData. Init site_count: " << genome_data.size() << endl;
//
//    t1 = clock();
//
//    SequencingFactory sequencing_factory(params);
//    printMemoryUsage("init factory");
//    sequencing_factory.CreateSequenceProbsVector(genome_data);
//
//    MutationModelMultiCategories modelMulti (2, evo_model0, sequencing_factory);
//
//    cout << "Time Create Mutation Model: " << ((clock() - t1) / CLOCKS_PER_SEC) << "\t" << (clock() - t1) << endl;
//
//    cout << "===== Done preprocess. Final site count: " << modelMulti.GetSiteCount() << endl;
//    printMemoryUsage("Created Mutation Model");
//    cout << "======= Setup EM ======" << endl;
//
//    const string &outfile_prefix = variables_map["outfile"].as<string>();
//
//    clock_t t_start, t_end;
//    t_start = clock();
//
//    std::chrono::time_point<std::chrono::system_clock> start, end;
//    start = std::chrono::system_clock::now();
//
//    EmAlgorithmThreadMutation em_algM(modelMulti, thread_count);
//    em_algM.SetOutfilePrefix(outfile_prefix);
//    printMemoryUsage("EM");
//    em_algM.Run();
//    em_algM.PrintSummary();
//    std::string summary = em_algM.GetEMSummary();
//    std::cout << "FinalSummary: " << summary << std::endl;
//
//
//    t_end = clock();
//    cout << "EM Clock time: " << (t_end - t_start) / CLOCKS_PER_SEC << "\t" << (t_end - t_start) << endl << endl;
//
//    end = std::chrono::system_clock::now();
//    std::chrono::duration<double> elapsed_seconds = end-start;
//    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
//    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
//
//    std::cout << "EM started at "<< std::ctime(&start_time) << "EM finished at " << std::ctime(&end_time)  << "EM elapsed time: " << elapsed_seconds.count() << "s\n";
//
//    printMemoryUsage("End EM");
//
//}


//
//void PostFilterGenomeData(GenomeData &genome_data) {
//
////    Custom filter, remove sample 47 51 44   ->  7 8 10
////
////    for (auto &data : genome_data) {
//    for (int g = genome_data.size()-1 ; g >= 0; --g) {
//
//        ModelInput &data = genome_data[g];
////        for (int i = 0; i < data.all_reads.size(); ++i) {
////            std::cout << i << ":" << data.all_reads[i].key << " ";
////        }
////        std::cout << "" << std::endl;
//        data.all_reads.erase(data.all_reads.begin()+10);
//        data.all_reads.erase(data.all_reads.begin()+7, data.all_reads.begin()+9);
//
////        for (int i = 0; i < data.all_reads.size(); ++i) {
////            std::cout << i << ":" << data.all_reads[i].key << " ";
////        }
////        std::cout << "" << std::endl;
////        data.all_reads.erase(data.all_reads.begin()+11);
////        data.all_reads.erase(data.all_reads.begin()+9);
////        data.all_reads.erase(data.all_reads.begin()+1, data.all_reads.begin()+7);
//
//        int sum = 0;
//
//        for (int j = 0; j < 4; ++j) {
//            sum += data.all_reads[0].reads[j];
//        }
//        if(sum < 6){
////            data.all_reads.clear();
////            std::cout << g << ":" << genome_data.size() << " ";
////
////            std::cout << genome_data.size() << "\t" << data.all_reads[0].key << "\t" << sum << std::endl;
////            for (int j = 0; j < 4; ++j) {
////                std::cout << data.all_reads[0].reads[j]  << std::endl;
////            }
//
//            genome_data.erase(genome_data.begin()+g);
////            break;
//
//        }
//        else {
//            int zero_count = 0;
//            for (int i = data.all_reads.size() - 1; i > 0; --i) {
//                int sum = 0;
//
//                for (int j = 0; j < 4; ++j) {
//                    sum += data.all_reads[i].reads[j];
//                }
////            std::cout << i << "\t" << data.all_reads[i].key << "\t" << sum <<std::endl;
////            std::cout << i << ":" << sum << " ";
//                if (sum < 6) {
////                data.all_reads.erase(data.all_reads.begin()+i);//FIXME: current model assume equal des, fail if do this
//                    data.all_reads[i] = 0;
//                    zero_count ++;
//                }
//
//            }
////        std::cout << "" << std::endl;
////        for (int i = 0; i < data.all_reads.size(); ++i) {
////            std::cout << i << ":" << data.all_reads[i].key << " ";
////        }
////        std::cout << "" << std::endl;
////        std::cout << "FINAL SIZE: " << data.all_reads.size() << std::endl;
////        std::exit(13);
//            if (zero_count == data.all_reads.size() -1) {
////                data.all_reads.clear();
////                std::cout << "FINAL SIZE: " << data.all_reads.size() << "\t" << zero_count << std::endl;
////                for (int i = 0; i < data.all_reads.size(); ++i) {
////                    std::cout << i << ":" << data.all_reads[i].key << " ";
////                }
////                 std::cout << "" << std::endl;
//
//                genome_data.erase(genome_data.begin() + g);
//
//            }
//        }
////        for (int i = 0; i < data.all_reads.size(); ++i) {
////            int sum = 0;
////            for (int j = 0; j < 4; ++j) {
////                sum += data.all_reads[i].reads[j];
////            }
//////            if(sum<6){
//////                std::cout << i << "\t" ;
//////                for (int j = 0; j < 4; ++j) {
//////                    std::cout << data.all_reads[i].reads[j] << " "  ;
//////                }
//////                std::cout << "" << std::endl;
//////            }
//////            std::cout << i << "\t" << data.all_reads[i].key << std::endl;
////        }
////        std::cout << "" << std::endl;
////        std::cout << "\n=======\n" << std::endl;
////        exit(32);
//    }
////FinalSummary: 5.57299e-01	6.02335e-06	1.32571e-04	9.99867e-01	-238817.82286	7.70999e-11
////EM Clock time: 32	32148156
//
//}


MutationModelMultiCategories CreateMutationModelMulti(GenomeData genome_data, ModelParams params) {


    clock_t t1;
    t1 = clock();


//    std::vector<SiteGenotypesIndex> sgi;
    printMemoryUsage("Before factory");
    SequencingFactory sequencing_factory(params);
    printMemoryUsage("init factory");

    t1 = clock();
    sequencing_factory.CreateSequenceProbsVector(genome_data);
    cout << "Time init seq latest: " << ((clock() - t1) / CLOCKS_PER_SEC) << "\t" << (clock() - t1) << endl;
    printMemoryUsage("after factory");


    MutationProb mutation_prob (params);
    F81 evo_model0(mutation_prob);


//    MutationModel mutation_model = MutationModel(evo_model0);
    MutationModelMultiCategories model (2, evo_model0, sequencing_factory);

//    MutationModel::AddGenotypeFactory(sequencing_factory);
//    std::cout << "SiteGenotypeIndex Size: " << sgi.size() << std::endl;
//    printMemoryUsage("add genotypes");
//    mutation_model.MoveSequenceProb(std::move(sgi));
//    printMemoryUsage("add seq probs");
    return model;

}

void CreateMutationModel(MutationModel &mutation_model, GenomeData &genome_data, ModelParams &params) {

    printMemoryUsage("Before factory");

    std::vector<SiteGenotypesIndex> sgi;
    SequencingFactory sequencing_factory(params);
    sequencing_factory.CreateSequenceProbsVector(sgi, genome_data);
    printMemoryUsage("after factory");

    MutationModel::AddGenotypeFactory(sequencing_factory);
    std::cout << "SiteGenotypeIndex Size: " << sgi.size() << std::endl;
    printMemoryUsage("Added genotypes");
    mutation_model.MoveSequenceProb(std::move(sgi));
    printMemoryUsage("Added seq probs");


}




//GenomeData getGenomeData(boost::program_options::variables_map variables_map) {
//
//
//    std::string file_name = variables_map["bam"].as<string>();// "zz_test_GenomeData_binary_subset";
////    PileupUtils::WriteGenomeDataToBinary(file_name, genome_data);
//    GenomeData genome_data;
//    PileupUtils::ReadGenomeDataFromBinary(file_name, genome_data);
//    return genome_data;
//
//}

int main(int argc, char** argv){
    Eigen::initParallel();//TODO: triple check this!!

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    boost::program_options::variables_map variables_map;
    BoostUtils::ParseCommandLinkeInput(argc, argv, variables_map);

//    SummariseRealData(variables_map);
//    exit(83);
    {
//        RefitData(genome_data, params);
//        RunEmWithRealDataOneStep(variables_map);

        RefitData(variables_map);
        int thread_count = variables_map["thread"].as<int>();
//        RunEmWithRealDataMultiThread(variables_map, thread_count);

//        RunEmWithRealDataOriginal(variables_map);

    }


    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "Program started at "<< std::ctime(&start_time) <<
            "Program finished at " << std::ctime(&end_time)  <<
            "Total elapsed time: " << elapsed_seconds.count() << "s" << std::endl;


    printMemoryUsage("End");

}
