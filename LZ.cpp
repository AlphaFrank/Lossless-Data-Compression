#include<iostream>
#include<string>
#include<cstdlib>
#include<cmath>
#include<list>
#include<bitset>
#include<deque>
#include<unordered_map>
#include<fstream>
#include<ctime>


// Initialize originalByte and outputByte for calculating saving rate
static int originalByte = 0;
static int outputByte = 0;
static bool fileExist = true;

// Declare a function
int handleMainArguments(int argc, char* argv[], int* N, int* L, int* S, std::string* inputFileName);

// A class that output bytes by taking bit-size inputs from users.
// It only output when the buffer is full.
class bitWriter{
	// The buffer
	std::list<int> queue;

public:
	// Constructor
	bitWriter(){}

	void writeInt(int value, int numberOfBits){
		// Convert value to binary representation
		std::bitset<14> bs(value);

		// Put bits into buffer
		for (int i=numberOfBits-1; i>=0; i--){
			queue.push_back(bs[i]);
		}

		// Check whether the buffer is ready to output
		checkReadyToOutput();
	}

	void writeChar(char value){
		// Convert value to binary representation
		std::bitset<8> bs(value);

		// Put bits into buffer
		for (int i=7; i>=0; i--){
			queue.push_back(bs[i]);
		}

		// Check whether the buffer is ready to output
		checkReadyToOutput();
	}

	// Flush the remaining bits in buffer
	void flush(){
		// Check whether there is remaining bit
		if (!queue.empty()){
			// Fill the buffer with meaningless bits
			while (queue.size() != 8){
				queue.push_back(0);
			}

			// Output it
			checkReadyToOutput();
		}
	}

private:
	void checkReadyToOutput(){
		// keep doing it until the size is less than 8
		while (queue.size() >= 8){

			// Fill the bitset with the first 8 bit in buffer
			std::bitset<8> bs;
			for (int i=7; i>=0; i--){
				bs[i] = queue.front();
				queue.pop_front();
			}

			// Convert it into long
			long toOutput = bs.to_ulong();

			// Convert it into char and output it
			std::cout << (char) toOutput;

			// Increment outputByte
			outputByte++;
		}
	}
};



// A data structure including a hash table and two double-ended queues.
// It is moderately quick when insert, delete and search
class slidingWindow{
	// Two parameters that decide the window and lookaheadbuffer size
	int W;
	int F;
	// Two useful parameters
	int hasRead;
	int hasCleaned;

	// Two double-ended queues and one hash table
	std::deque<char> window;
	std::unordered_map< std::string, std::deque<int> > lookupTable;
	std::deque<char> lookaheadBuffer;

	// Input file stream
	std::ifstream in;

public:
	//Constructor
	slidingWindow(int w, int f, std::string name) : W(w), F(f) {
		in.open(name);
		if (!in.good()){
			fileExist = false;
			std::cerr << "Input file does not exist." << std::endl;
		}
	}

	//Deconsructor
	~slidingWindow(){
		in.close();
	}



	//Initialization
	void initialize(){
		hasRead = 0;
		hasCleaned = 0;

		// Fill the window with W-F '\0's		
		for (int i=0; i<W-F; i++){
			window.push_back('\0');
		}

		// Fill the window and the lookaheadbuffer with F chars
		char c;
		for (int i=0; i<F; i++){
			if (in.get(c)){
				lookaheadBuffer.push_back(c);
				window.push_back(c);
				originalByte++;
			}

		}



	}

	// Move the window forward
	void move(int step){
		// Try get chars from the input file stream and push the chars to the end of window and lookaheadbuffer
		char c;
		for (int i=0; i<step; i++){
			if (in.get(c)){
				lookaheadBuffer.push_back(c);
				window.push_back(c);
				originalByte++;
			}
		}

		// Update the lookup table
		for (int i=0; i<step; i++){
			if (i+1 < lookaheadBuffer.size()){
				std::string key = str(lookaheadBuffer[i], lookaheadBuffer[i+1]);
				int value = hasRead+i;
				lookupTable[key].push_back(value);
			}
		}

		// Update hasRead
		hasRead += step;

		// Pop the front elements from lookaheadbuffer to make its size equal to F
		for (int i=0; i<step; i++){
			lookaheadBuffer.pop_front();
		}
		
		// Pop the front elements from window to make its size equal to W
		for (int i=0; i<step; i++){
			if (hasCleaned >= W-F){
				std::string key = str(window[0], window[1]);
				lookupTable[key].pop_front();
			}
			window.pop_front();
			hasCleaned++;
		}
	}

	// Find the max match
	// If there is a match of length >= 2, return <length, offset>
	// Else return <0, 1, first char in lookaheadbuffer>
	std::pair<int, int> match(){
		// If the lookaheadbuffer's size is 1, just return the triple
		if (lookaheadBuffer.size()==1){
			return std::pair<int, int>(0, (int) lookaheadBuffer[0]);
		}

		// Form a key using the first two chars of lookaheadbuffer
		std::string key = str(lookaheadBuffer[0], lookaheadBuffer[1]);
		// If the cannot find it in lookup table, return the triple
		if (lookupTable[key].empty()){
			return std::pair<int, int>(0, (int) lookaheadBuffer[0]);
		}
		// Else using a simple maximum-finding algorithm to find the longest match
		else{
			int max = 0;
			int position;
			int score;
			for(int i=0; i<lookupTable[key].size(); i++){
				score = matchScore(lookupTable[key][i]);
				if (score > max){
					max = score;
					position = lookupTable[key][i];
				}
			}
			return std::pair<int, int>(max, hasRead-position);
		}
	}

	// return whether lookaheadbuffer is empty
	bool done(){
		return lookaheadBuffer.empty();
	}







private:
	// Given two chars, return a string consisting of these two chars
	// It is useful to form a string-type key from two chars
	std::string str(char c1, char c2){
		std::string s;
		s.push_back(c1);
		s.push_back(c2);
		return s;
	}
	
	// Return a match length given a start point in the window.
	int matchScore(int index){
		int result = 0;
		int current = index - (hasRead-W+F);
		for (int i=0; i<lookaheadBuffer.size(); i++){
			if (window[current] != lookaheadBuffer[i]){
				break;
			} else {
				current++;
				result++;
			}
		}
		return result;
	}
};










int main(int argc, char* argv[]){
	// Start timeing
	std::clock_t start = std::clock();

	// Set default values for parameters
	int N = 11;
	int L = 4;
	int S = 3;	
	std::string inputFileName("");

	// Handle everything in arguments
	if (handleMainArguments(argc, argv, &N, &L, &S, &inputFileName) == 0){
		return 0;
	}

	// Initialize all paramenters
	int W = exp2(N);
	int F = exp2(L)-1;
	int longestString = exp2(S)-1;

	// Initialize the bitWriter
	bitWriter bw = bitWriter();

	// Output the first three bytes (N, L and S)
	bw.writeInt(N, 8);
	bw.writeInt(L, 8);
	bw.writeInt(S, 8);


	// Construct and initialize the sliding window
	slidingWindow sw(W, F, inputFileName);
	if (!fileExist){
		return 0;
	}
	sw.initialize();

	// Construct a buffer to store triples. e.g. <0,1,a><0,1,b> => <0,1,ab>
	std::deque<char> buf;
	// pr is the response from the match function of sw. It tells me how to write.
	std::pair<int, int> pr; 
	// Stop when the lookaheadbuffer is empty
	while (!sw.done()){
		pr = sw.match();

		if (pr.first==0){
			// Put the char into buf instead writing it directly
			buf.push_back((char) pr.second);

			// Check whether ready to write
			if (buf.size() == longestString){
				bw.writeInt(0, L);
				bw.writeInt(longestString, S);
				for (int i=0; i<longestString; i++){
					bw.writeChar(buf[i]);
				}
				buf.clear();
			}

			// Only move one when there is no match >= 2
			sw.move(1);
		} else {
			// Write triples before write doubles 
			if (!buf.empty()){
				bw.writeInt(0, L);
				bw.writeInt(buf.size(), S);
				for (int i=0; i<buf.size(); i++){
					bw.writeChar(buf[i]);
				}
				buf.clear();
			}

			bw.writeInt(pr.first-1, L);
			bw.writeInt(pr.second, N);

			// Move "length" 
			sw.move(pr.first);
		}
		
	}

	// Make sure the buf is clear
	if (!buf.empty()){
		bw.writeInt(0, L);
		bw.writeInt(buf.size(), S);
		for (int i=0; i<buf.size(); i++){
			bw.writeChar(buf[i]);
		}
		buf.clear();
	}


	// Write <0,0> to indicate stop
	bw.writeInt(0, L);
	bw.writeInt(0, S);
	// Make sure no bits are in the buffer.
	bw.flush();

	// Output useful information
	std::cerr << "N = " << N << ", L = " << L << ", S = " << S << std::endl;
	std::cerr << "Original byte = " << originalByte << std::endl;
	std::cerr << "Output byte = " << outputByte << std::endl;
	std::cerr << "The compressed version is " << ((float) outputByte)/ ((float) originalByte)*100 << "\% of the original file." << std::endl;

	double duration = (std::clock()-start) / (double) CLOCKS_PER_SEC;
	std::cerr << "Compressing duration: " << duration << " seconds" << std::endl;

	return 0;
}











int handleMainArguments(int argc, char* argv[], int* N, int* L, int* S, std::string* inputFileName){
	int hasFileName = 0;
	int j;
	std::string buffer("");
	int bufferInt;

	// Loop through all arguments 
	for (int i=1; i<argc; i++){
		// If the argument start with '-'
		if (argv[i][0] == '-'){
			// If the argument is trying to change N
			if (argv[i][1] == 'N'){
				j=3;
				while (argv[i][j] != '\0'){
					buffer += argv[i][j];
					j++;	
				}
				bufferInt = atoi(buffer.c_str());
				// Make sure the new N is in range
				if ((bufferInt>=9) && (bufferInt<=14)){
					*N = bufferInt;
				} else {
					std::cerr << "N out of range" << std::endl;
					return 0;		
				}
				buffer.clear();
			}
			// If the argument is trying to change L
            else if (argv[i][1] == 'L'){
                j=3;
                while (argv[i][j] != '\0'){
                    buffer += argv[i][j];
                    j++;
                }
                bufferInt = atoi(buffer.c_str());
                // Make sure the new L is in range
				if ((bufferInt>=3) && (bufferInt<=4)){
                    *L = bufferInt;
                } else {
                    std::cerr << "L out of range" << std::endl;
                	return 0;
				}                                                                 
				buffer.clear();
            }
			// If the argument is trying to change S
            else if (argv[i][1] == 'S'){
                j=3;
                while (argv[i][j] != '\0'){
                    buffer += argv[i][j];
                    j++;
                }
                bufferInt = atoi(buffer.c_str());
				// Make sure the new S is in range
                if ((bufferInt>=1) && (bufferInt<=5)){
                    *S = bufferInt;
                } else {
                    std::cerr << "S out of range" << std::endl;
                	return 0;
				}
                buffer.clear();
        	}
			// If the given parameter to change does not exist, report error
			else{
				std::cerr << "Parameter not valid" << std::endl;
				return 0;
			}		
		}
		// If the argument is trying to give file name
		else{
			// If not given, assign the arugument to file name	
			if (hasFileName == 0){
				hasFileName = 1;
				j=0;
				while (argv[i][j] != '\0'){
					*inputFileName += argv[i][j];
					j++;
				}
			}
			// If already got one, report error
			else{
				std::cerr << "Mutiple file names" << std::endl;
				return 0;
			}
			
		}
	}

		
	// Report error if no file name is given
	if (hasFileName == 0){
		std::cerr << "No file name" << std::endl;
		return 0;
	}

	return 1;
}
















