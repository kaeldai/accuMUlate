/*
 * mutation_model.h
 *
 *  Created on: 12/15/14
 *      Author: Steven Wu
 */


#pragma once
#ifndef MUTATION_MODEL_H_
#define MUTATION_MODEL_H_


#include <vector>
#include <iostream>
#include <unordered_map>

#include "model.h"
#include "mutation_prob.h"
#include "site_prob.h"
#include "sequence_prob.h"
#include "distributions/DirichletMultinomialDistribution.h"
#include "evolution_models/EvolutionModel.h"
#include "lookup.h"
#include "site_prob.h"


typedef std::array<std::array<double, 4>, 10> Std2DArray;

class MutationModel {

public:
    MutationModel(MutationProb mutation_prob, EvolutionModel evo_model);

    MutationModel(EvolutionModel &evo_model0);



    void AddCache(std::unordered_map<double, std::array<double, 4>> &allDMaps4,
            std::unordered_map<double,  Std2DArray>  &cache_data_transition);
    virtual ~MutationModel() {
    }

    void AddCache2(
//            std::unordered_map<uint64_t, std::array<double, 10>> prob, std::unordered_map<uint64_t, std::array<double, 10>> stat,
//            std::unordered_map<uint64_t, std::pair<std::array<double, 10>, std::array<double, 10> > > all,
            std::unordered_map<uint64_t, std::array<std::array<double, 2>, 10> > &all_2);

//    std::unordered_map<uint64_t, std::array<double, 10>> cache_read_data_to_prob;
//    std::unordered_map<uint64_t, std::array<double, 10>> cache_read_data_to_stat;
//    std::unordered_map<uint64_t, std::pair<std::array<double, 10>, std::array<double, 10> > > cache_read_data_to_all;
    std::unordered_map<uint64_t, std::array<std::array<double, 2>, 10> > cache_read_data_to_all;
public:
    MutationModel() {
    }
    void UpdateExpBeta(double expBeta);

//    void UpdateModel(EvolutionModel &evo_model);

    void CalculateLikelihood(SequenceProb &sequence_prob);

    void CalculateAncestorToDescendant(int site_index, double &prob_reads, double &all_stats_diff);

    void CalculateAllDescendantGivenAncestor(int a, double &product_prob_given_ancestor, double &summary_stat_diff_ancestor);

    void CalculateOneDescendantGivenAncestor(int anc_index10, HaploidProbs prob_reads_given_descent, double &prob_reads_d_given_a, double &summary_stat_same, double &summary_stat_diff);

private:

//    DiploidProbs ancestor_genotypes;
//    std::vector<HaploidProbs> all_descendant_genotypes;

    MutationRate mutation_rate;
    MutationMatrix transition_matrix_a_to_d;

    Array4D frequency_prior;
    Array10D ancestor_prior;

    int descendant_count;


//    void UpdateMuProb(MutationProb mutation_prob);
//
//    void UpdateTransitionMatrix(EvolutionModel evo_model);



//    std::unordered_map<double, std::array<double, 4>> cache_data;
//    std::unordered_map<double,  Std2DArray> cache_data_transition;

    std::unordered_map<uint64_t, HaploidProbs> map_rd_key_to_haploid;
//    ReadDataVector all_descendant_data;


//    std::unordered_map<uint64_t, int> map;
//    std::vector<std::pair<uint64_t, int>> vec;


//    void CalculateAncestorToDescendantOrig(double &prob_reads, double &all_stats_same, double &all_stats_diff);
//
//    void CalculateAllDescendantGivenAncestorOrig(int a, double &product_prob_given_ancestor, double &summary_stat_same_ancestor, double &summary_stat_diff_ancestor);
//
//    void CalculateOneDescendantGivenAncestorOrig(int anc_index10, HaploidProbs prob_reads_given_descent, double &prob_reads_d_given_a, double &summary_stat_same, double &summary_stat_diff);
//
//    void CalculateLikelihoodOriginal(SequenceProb &sequence_prob, double &prob, double &stat_same, double &stat_diff);
//
//    void CalculateOneDescendantGivenAncestorCache(int anc_index10, HaploidProbs prob_reads_given_descent, double &prob_reads_d_given_a, double &summary_stat_same, double &summary_stat_diff);

public:
    void CacheLoopDes(int a, double &product_prob_given_ancestor, double &summary_stat_diff_ancestor);

    void CacheLoopMap(int a, double &product_prob_given_ancestor, double &summary_stat_diff_ancestor);

    int GetSiteCount() const {
        return site_count;
    }

    void NoCacheCalculateDes(int site_index, int a, double &product_prob_given_ancestor, double &stat_same, double &summary_stat_diff_ancestor);


    void CacheLoopDesAll(int site_index, int anc_index, double &product_prob_given_ancestor, double &summary_stat_diff_ancestor);

    void AddSequenceProb(std::vector<SequenceProb> &all);
    std::vector<SequenceProb> all_sequence_prob;

    EvolutionModel *evo_model;

    void InitCache();
    void UpdateCache();

    std::vector<std::array<double, 10>> all_ancestor_genotype_prior;
    int site_count;
};


#endif //MUTATION_MODEL_H_
