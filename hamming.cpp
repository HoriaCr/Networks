#include <iostream>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <cassert>
#include <ctime>
#include <cstdlib>

using namespace std;


class BitSet {
        uint32_t size;
        uint32_t *data;
        
        uint32_t getIntLen() const {
            return  (size >> 6) + ((size & 31) > 0);
        }


    public:

        BitSet(uint32_t size_) : size(size_) {
            data = new uint32_t[getIntLen()];
            memset(data, 0, getIntLen() * sizeof(uint32_t));
        }

        BitSet(const string& bitStr) {
            size = bitStr.size();
            data = new uint32_t[getIntLen()];
            memset(data, 0, getIntLen() * sizeof(uint32_t));
            for (uint32_t i = 0; i < size; i++) {
                assert('0' <= bitStr[i] && bitStr[i] <= '1');
                data[i >> 6] |= (bitStr[i] - '0') << (i & 31);
            }
        }

        BitSet(const BitSet& other) {
            size = other.size;
            data = new uint32_t[getIntLen()];
            memcpy(data, other.data, getIntLen() * sizeof(uint32_t));

        }

        BitSet &operator = (const BitSet& other) {
            if (this != &other) {
                size = other.size;
                data = new uint32_t[getIntLen()];
                memcpy(data, other.data, getIntLen() * sizeof(uint32_t));
            }
            return *this;
        }
        
        ~BitSet() {
            delete[] data;
        }

        uint32_t Size() { 
            return size;
        }

        void operator ^= (uint32_t i) {
            try {
                if (i >= size) {
                    throw out_of_range("Out of bounds!");
                }
                data[i >> 6] ^= 1 << (i & 31);
            } 
            catch(const out_of_range& e) {
                cerr << e.what() << "\n";
            }
        }

        bool xorSum() {
            uint32_t ret = 0;
            for (uint32_t i = 0; i < size; i++) {
                ret ^= __builtin_popcount(data[i]);
            }

            return ret == true;
        }

        uint32_t computeParities() const {
            uint32_t powerMask = 0;
            for (uint32_t i = 0; (1U << i) <= size; i++) {
                // take first 2^i skip next 2^i
                for (uint32_t j = (1 << i) - 1; j < size; j += 1 << (i + 1)) { 
                    uint32_t r = min(size,j + (1 << i));
                    // compute parity for 2^i
                    for (uint32_t k = j; k < r; k++) {
                        powerMask ^= (data[k >> 6] >> (k & 31) & 1) << i;
                    }
                }
            }
            return powerMask;
        }

        BitSet getHammingCode() const {
            uint32_t newSize = size;
            for (uint32_t i = 0; (1U << i) <= newSize; i++) {                
                  newSize++;
            }
            
            BitSet ret(newSize);
            for (uint32_t i = 0, j = 0; i < size; i++) {
                // skip powers of two
                while ((j & (j + 1)) == 0) j++; 
                cout << i  << " " << j << ":" << (data[i >> 6] >> (i & 31) & 1) << "\n";
                // copy
                ret.data[j >> 6] |= (data[i >> 6] >> (i & 31) & 1) << (j & 31); 
                j++;
            }

            uint32_t powerMask = ret.computeParities();

            for (uint32_t i = 0; (1U << i) <= newSize; i++) {
                uint32_t j = (1 << i) - 1;
                // set parities at powers of 2
                ret.data[j >> 6] |= ((powerMask >> i) & 1) << (j & 31); 
            }
            return ret;
        }

        string toString() const {
            string ret(size,'0');
            for (uint32_t i = 0; i < size; i++) {
                ret[i] += (data[i >> 6] >> (i & 31) & 1);
            }

            return ret;
        }

        // returns -1 if no error is found
        int detectError() const {
            return computeParities() - 1;
        }

        void fixError() {
            int e = detectError();
            if (e != -1) {
                *this ^= e;
            }
        }

        friend ostream& operator << (ostream& out,const BitSet& b) {
            out << b.toString(); 
            return out;
        }
};

int main() {
    string code = "01001101";
    BitSet s(code);
    BitSet h = s.getHammingCode();
    cout << s << "\n" << h << "\n";
    srand(static_cast<uint32_t>(time(0)));
    uint32_t randPos = rand() % h.Size();
    string correctCode = h.toString();
    // break a bit at a random position
    h ^= randPos;
    h.fixError();
    assert(h.toString() == correctCode);
}
