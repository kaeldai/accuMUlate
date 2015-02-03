#include "em_algorithm_mutation.h"
#include "em_summary_stat_mutation_v1.h"


EmAlgorithmMutation::EmAlgorithmMutation(std::vector<std::unique_ptr<EmModel>> &model_ptr) : EmAlgorithm(model_ptr) {

    InitWithModel();

}


EmAlgorithmMutation::~EmAlgorithmMutation() {

}

void EmAlgorithmMutation::Run() {

    em_stat_local_single->print();
    size_t i = 0;
    bool isConverged = true;
    while(isConverged){
        ExpectationStepModelPtr();
        MaximizationStep();
        isConverged = EmStoppingCriteria(i);
        i++;
    }
}


void EmAlgorithmMutation::InitialiseParameters() {
    double lower_bound = 1e-10;
    double upper_bound = 0.9;

    if (num_category == 2) {
        parameters = {upper_bound, lower_bound};
    }
    else {
        std::cout << "Not yet implemented for more than 2 categories" << std::endl;
        exit(222);
        //TODO: Should throw exception instead of exit, this will do for now
    }

//    em_model_ptr->at(0)->
}


void EmAlgorithmMutation::InitialiseSummaryStat() {

    em_stat_local_single = std::unique_ptr<EmSummaryStat>(new EmSummaryStatMutationV1());
    em_stat_local_single->print();

    temp_stats = std::vector<std::vector<double>>(num_category);
    for (size_t i = 0; i < num_category; ++i) {
        all_em_stats.emplace_back(new EmSummaryStatMutationV1());
//        all_em_stats.emplace_back(new EmSummaryStatMutationV1());
        temp_stats[i] = std::vector<double>(em_stat_local_single->GetStatCount());
    }


}


void EmAlgorithmMutation::ExpectationStepCustom(size_t data_index, size_t category_index,
        double &sum_prob, std::vector<double> &temp_stat) {
    std::cout << "Error!! should NOT call ExpectationStepCustom here" << std::endl;
    exit(40);
}