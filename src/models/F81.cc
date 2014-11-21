#include <iostream>
#include "F81.h"

F81::~F81() {
}

F81::F81(double mu, vector<double> freq) : EvolutionModel(mu) {
    cout << "F81\n";
    this->freqs = freq;
    beta0 = 1.0;
	for (auto d : freqs) {
		beta0 -= d * d;
	}
    beta0 = 1/ beta0;
    cout << "done beta\n";
    UpdateTransitionMatrix();
    cout << "done F81\n";
}


void F81::UpdateTransitionMatrix() {

    exp_beta = exp(-beta0 * mu);

    Eigen::Matrix4d m;
    for (int i : { 0, 1, 2, 3 }) {
        double prob = freqs[i] * (1.0 - exp_beta);
        for (int j : { 0, 1, 2, 3 }) {
            m(j, i) = prob;
        }
        m(i, i) += exp_beta;
    }

    //TODO: use col row to assign them
    transition_matrix_a_to_d = MutationMatrix::Zero();
    for (int i : { 0, 1, 2, 3 }) {
        for (int j : { 0, 1, 2, 3 }) {
            int index16 = i * 4 + j;
            for (int k : { 0, 1, 2, 3 }) {
//                conditional_prob(i * 4 + j, k) = 0.0;
                transition_matrix_a_to_d(index16, k) += 0.5 * m(i, k);
                transition_matrix_a_to_d(index16, k) += 0.5 * m(j, k);
            }
        }
    }
    
    std::cout << "Calculate F81:\n" << mu <<  "\t" << beta0 << "\t" << exp_beta << "\n" <<
             freqs[0] << freqs[1] << "\n" << m << "\n========================\n" << transition_matrix_a_to_d << endl;

}





/* Old code from other places
    mutation = MutationAccumulation(params, false);
	MutationMatrix mt = MutationAccumulation(params, true);
	non_mutation = mutation - mt;

MutationMatrix MutationProb::MutationAccumulation2(bool and_mut) {

	using namespace Eigen;
	double beta = CalculateBeta();
//	printf("beta0:%.10f\n", beta0); //0.9999999852
	Eigen::Matrix4d m;
	Eigen::Matrix4d m2 = Eigen::Matrix4d::Zero();


	Matrix3d m3 = Matrix3d::Identity();
	m3.row(1) = Vector3d(4,5,6);
//	std::cout << m3 << std::endl;

	for (int i : { 0, 1, 2, 3 }) {
		for (int j : { 0, 1, 2, 3 }) {
			m(i, j) = frequency_prior[i] * (1.0 - beta);
		}
		m(i, i) += beta;
	}
//	element wise operation
	for (int i : { 0, 1, 2, 3 }) {
		double c = frequency_prior[i] * (1.0 - beta);
//		c = (i+1) * 1.1;
		m2.row(i) = Vector4d::Constant(c);
	}
	m2.diagonal() += Vector4d::Constant(beta);
//	std::cout << m << std::endl;
//	std::cout << "\n" << m2 << std::endl;

	//cerr << m << endl;
	MutationMatrix result;
	for (int i : { 0, 1, 2, 3 }) {
		for (int j : { 0, 1, 2, 3 }) {
			for (int k : { 0, 1, 2, 3 }) {
				result(i * 4 + j, k) = 0.0;
				if (!and_mut || i != k)
					result(i * 4 + j, k) += 0.5 * m(i, k);
				if (!and_mut || j != k)
					result(i * 4 + j, k) += 0.5 * m(j, k);
			}
		}
	}

	MutationMatrix result2 = MutationMatrix::Zero();;
		for (int i : { 0, 1, 2, 3 }) {
			for (int j : { 0, 1, 2, 3 }) {
				for (int k : { 0, 1, 2, 3 }) {
//					result(i * 4 + j, k) = 0.0;
					if (!and_mut || i != k)
						result2(i * 4 + j, k) += 0.5 * m2(i, k);
					if (!and_mut || j != k)
						result2(i * 4 + j, k) += 0.5 * m2(j, k);
				}
			}
		}
//	printf("%d\n", and_mut);
//	std::cout << result << std::endl;
//	std::cout << "\n" << std::endl;

//	std::cout << (result2 - result).sum() << "<-Should be 0" << std::endl;



	return result;
}

MutationMatrix MutationProb::MutationAccumulation(const ModelParams &params, bool and_mut) {

	double beta = CalculateBeta();
//	printf("beta0:%.10f\n", beta0); //0.9999999852
	Eigen::Matrix4d m;
	for (int i : { 0, 1, 2, 3 }) {
		for (int j : { 0, 1, 2, 3 }) {
			m(i, j) = frequency_prior[i] * (1.0 - beta);
		}
		m(i, i) += beta;
	}
//	std::cout << m << std::endl;

	//cerr << m << endl;
	MutationMatrix result;
	for (int i : { 0, 1, 2, 3 }) {
		for (int j : { 0, 1, 2, 3 }) {
			for (int k : { 0, 1, 2, 3 }) {
				result(i * 4 + j, k) = 0.0;
				if (!and_mut || i != k)
					result(i * 4 + j, k) += 0.5 * m(i, k);
				if (!and_mut || j != k)
					result(i * 4 + j, k) += 0.5 * m(j, k);
			}
		}
	}
//	printf("%d\n", and_mut);
//	std::cout << result << std::endl;

	return result;
}


 */