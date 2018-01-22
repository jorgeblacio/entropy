#include "entropy.h"

std::map <int, double> counters;  
std::queue <op_t> window;  
int windowSize;
double sum;
double entropy;

int initEntropy(int _windowSize){
	if (_windowSize <= 0) {
		return -1;
	}

	windowSize = _windowSize;
	sum = 0.0;
	entropy = 0.0;
	
	
	return 0;
}

void initializeCounterIfNecessary(op_t element) {
    uint64_t label = element.key_hv;

    if (counters.find(label) == counters.end()) {
        counters[label] = 0.0;
    }
}

void updateEntropyWithoutElement(op_t element) {
    uint64_t label = element.key_hv;
    double value = 1.0;
    sum -= value;
    counters[label] -= value;

    if (sum == 0.0) {
        entropy = 0.0;
    } else {
        double pi = counters[label] / (sum + value);
        double piPrev = (counters[label] + value) / (sum + value);
        double ratio = sum / (sum + value);
        double ratioInv = (sum + value) / sum;

        entropy = ratioInv * (entropy + piPrev * log_n(piPrev) - pi * log_n(pi)) + log_n(ratio);
    }   
}

void removeLastElement() {
    op_t lastElement = window.front();
    updateEntropyWithoutElement(lastElement);
    window.pop();
}

void updateWindowContent(op_t element) {
    window.push(element);
    if (window.size() > windowSize) {
        removeLastElement();
    }
}

void updateEntropy(op_t element) {
    uint64_t label = element.key_hv;
    double pi = counters[label] / sum;
    double value = 1.0;
    double piPrev = (counters[label] - value) / sum;
    double ratio = (sum - value) / sum;


    entropy = ratio * (entropy - log_n(ratio)) - pi * log_n(pi) + (piPrev * log_n(piPrev));
}

void updateCounters(op_t element) {
    uint64_t label = element.key_hv;
    double value = 1.0;

    sum += value;
    counters[label] += value;
}


void processEntropy(op_t element) {
        initializeCounterIfNecessary(element);
        // NOTE: It is important that we update the counters *before* updating
        // the entropy, and update entropy before updating window content (see
        // [1] for details).
        updateCounters(element);
        updateEntropy(element);
        updateWindowContent(element);
}  

double log_n(double t){
    return t > 0 ? log(t) : 0.0;
} 

double getEntropyValue(){
    return entropy;
}
