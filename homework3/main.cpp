#include <iostream>
#include <fstream>
#include <string>

using namespace std;

ifstream fin("input.in");
ifstream pr("proof_not.in");
ofstream fout("output.out");

void write_predic() {
    fout << "a=b->a=c->b=c" << std::endl;
    fout << "(a=b->a=c->b=c)->(A->A->A)->(a=b->a=c->b=c)" << std::endl;
    fout << "A->A->A" << std::endl;
    fout << "(A->A->A)->(a=b->a=c->b=c)" << std::endl;
    fout << "(A->A->A)->@a(a=b->a=c->b=c)" << std::endl;
    fout << "(A->A->A)->@b@a(a=b->a=c->b=c)" << std::endl;
    fout << "(A->A->A)->@c@b@a(a=b->a=c->b=c)" << std::endl;
    fout << "@c@b@a(a=b->a=c->b=c)" << std::endl;
    fout << "a+0=a" << std::endl;
    fout << "(a+0=a)->(A->A->A)->(a+0=a)" << std::endl;
    fout << "(A->A->A)->(a+0=a)" << std::endl;
    fout << "(A->A->A)->@a(a+0=a)" << std::endl;
    fout << "@a(a+0=a)" << std::endl;
    fout << "a=b->a\'=b\'" << std::endl;
    fout << "(a=b->a\'=b\')->(A->A->A)->(a=b->a\'=b\')" << std::endl;
    fout << "(A->A->A)->(a=b->a\'=b\')" << std::endl;
    fout << "(A->A->A)->@a(a=b->a\'=b\')" << std::endl;
    fout << "(A->A->A)->@b@a(a=b->a\'=b\')" << std::endl;
    fout << "@b@a(a=b->a\'=b\')" << std::endl;
    fout << "a+b\'=(a+b)\'" << std::endl;
    fout << "(a+b\'=(a+b)\')->(A->A->A)->(a+b\'=(a+b)\')" << std::endl;
    fout << "(A->A->A)->(a+b\'=(a+b)\')" << std::endl;
    fout << "(A->A->A)->@a(a+b\'=(a+b)\')" << std::endl;
    fout << "(A->A->A)->@b@a(a+b\'=(a+b)\')" << std::endl;
    fout << "@b@a(a+b\'=(a+b)\')" << std::endl; //25
}
void write_equal(std::string G) {
    G = "(" + G + ")";
    fout << "@c@b@a(a=b->a=c->b=c)->@b@a(a=b->a=" << G << "->b=" << G << ")" << std::endl;//45
    fout << "@b@a(a=b->a=" << G << "->b=" << G << ")" << std::endl;
    fout << "@b@a(a=b->a=" << G << "->b=" << G << ")->@a(a=" << G << "->a=" << G << "->" << G << "=" << G << ")" << std::endl;
    fout << "@a(a=" << G << "->a=" << G << "->" << G << "=" << G << ")" << std::endl;
    fout << "@a(a=" << G << "->a=" << G << "->" << G << "=" << G << ")->(" << G << "+0=" << G << "->" << G <<"+0=" << G << "->" << G << "=" << G << ")" << std::endl;
    fout << G << "+0=" << G << "->" << G <<"+0=" << G << "->" << G << "=" << G << std::endl;
    fout << "@a(a+0=a)->(" << G << "+0=" << G << ")" << std::endl;
    fout << G << "+0=" << G << std::endl;
    fout << G <<"+0=" << G << "->" << G << "=" << G << std::endl;
    fout << G << "=" << G << std::endl;
}
void add_one(std::string A, std::string K, std::string B) {
    A = "(" + A + ")";
    K = "(" + K + ")";
    B = "(" + B + ")";
    fout << "@b@a(a=b->a\'=b\')->@a(a=" << B << "->a\'=" << B << "\')" << std::endl;
    fout << "@a(a=" << B << "->a\'=" << B << "\')" << std::endl; //30
    fout << "@a(a=" << B << "->a\'=" << B << "\')->((" << A << "+" << K << ")=" << B << "->(" << A << "+" << K << ")\'=" << B << "\')" << std::endl;
    fout << "(" << A << "+" << K << ")=" << B << "->(" << A << "+" << K << ")\'=" << B << "\'" << std::endl;
    fout << "(" << A << "+" << K << ")\'=" << B << "\'" << std::endl;
    fout << "@b@a(a+b\'=(a+b)\')->@a(a+" << K << "\'=(a+" << K << ")\')" << std::endl;
    fout << "@a(a+" << K << "\'=(a+" << K << ")\')" << std::endl;//35
    fout << "@a(a+" << K << "\'=(a+" << K << ")\')->(" << A << "+" << K << "\'=(" << A << "+" << K << ")\')" << std::endl;
    fout << A << "+" << K << "\'=(" << A << "+" << K << ")\'" << std::endl;
    std::string a = "(" + A + "+" + K + "\')", b = "(" + A + "+" + K + ")\'";
    fout << "@c@b@a(a=b->a=c->b=c)->@b@a(a=b->a=" << a << "->b=" << a << ")" << std::endl;
    fout << "@b@a(a=b->a=" << a << "->b=" << a << ")" << std::endl;//40
    fout << "@b@a(a=b->a=" << a << "->b=" << a << ")->@a(a=" << b << "->a=" << a << "->" << b << "=" << a << ")" << std::endl;
    fout << "@a(a=" << b << "->a=" << a << "->" << b << "=" << a << ")" << std::endl;
    fout << "@a(a=" << b << "->a=" << a << "->" << b << "=" << a << ")->(" << a << "=" << b << "->" << a << "=" << a << "->" << b << "=" << a << ")" << std::endl;
    fout << a << "=" << b << "->" << a << "=" << a << "->" << b << "=" << a << std::endl;
    write_equal(a);
    fout << a << "=" << a << "->" << b << "=" << a << std::endl;
    fout << b << "=" << a << std::endl;
    a = b;
    b = "(" + A + "+" + K + "\')";
    std::string c = B + "\'";
    fout << "@c@b@a(a=b->a=c->b=c)->@b@a(a=b->a=" << c << "->b=" << c << ")" << std::endl;
    fout << "@b@a(a=b->a=" << c << "->b=" << c << ")" << std::endl;
    fout << "@b@a(a=b->a=" << c << "->b=" << c << ")->@a(a=" << b << "->a=" << c << "->" << b << "=" << c << ")" << std::endl;
    fout << "@a(a=" << b << "->a=" << c << "->" << b << "=" << c << ")" << std::endl;
    fout << "@a(a=" << b << "->a=" << c << "->" << b << "=" << c << ")->(" << a << "=" << b << "->" << a << "=" << c << "->" << b << "=" << c << ")" << std::endl;
    fout << a << "=" << b << "->" << a << "=" << c << "->" << b << "=" << c << std::endl;
    fout << a << "=" << c << "->" << b << "=" << c << std::endl;
    fout << b << "=" << c << std::endl;
}
void reverse(std::string A, std::string B) {
    A = "(" + A + ")";
    B = "(" + B + ")";
    fout << "@c@b@a(a=b->a=c->b=c)->@b@a(a=b->a=" << A << "->b=" << A << ")" << std::endl;
    fout << "@b@a(a=b->a=" << A << "->b=" << A << ")" << std::endl;
    fout << "@b@a(a=b->a=" << A << "->b=" << A << ")->@a(a=" << B << "->a=" << A << "->" << B << "=" << A << ")" << std::endl;
    fout << "@a(a=" << B << "->a=" << A << "->" << B << "=" << A << ")" << std::endl;
    fout << "@a(a=" << B << "->a=" << A << "->" << B << "=" << A << ")->(" << A << "=" << B << "->" << A << "=" << A << "->" << B << "=" << A << ")" << std::endl;
    fout << A << "=" << B << "->" << A << "=" << A << "->" << B << "=" << A << std::endl;
    fout << A << "=" << A << "->" << B << "=" << A << std::endl;
    write_equal(A);
    fout << B << "=" << A << std::endl;
}
void write_proof(std::string A, std::string B, std::string C, int c) {
    write_predic();
    std::string a = A, b = A, k = "0";
    fout << "@a(a+0=a)->(" << A << "+0=" << A << ")" << std::endl;
    fout << A << "+0=" << A << std::endl;//28
    for (int i = 0; i < c; i++) {
        add_one(a, k, b);
        b = b + "\'";
        k = k + "\'";
    }
    fout << "(((" << A << ")+(" << C << "))=(" << B << "))->?p(((" << A << ")+p)=(" << B << "))" << std::endl;
    fout << "?p(((" << A << ")+p)=(" << B << "))" << std::endl;
}

int main() {
    int a, b;
    fin >> a >> b;
    if (b >= a) {
        string A = "0", B = "0", C = "0";
        for (int i = 0; i < a; i++)
            A = A + "\'";
        for (int i = 0; i < b; i++)
            B = B + "\'";
        for (int i = 0; i < b - a; i++)
            C = C + "\'";
//        fout << A << " " << B << " " << C << std::endl;
        write_proof(A, B, C, b - a);
    } else {
        string B = "", C = "";
        for (int i = 0; i < b; i++)
            B = B + "\'";
        for (int i = 0; i < b - a - 1; i++)
            C = C + "\'";
        string tmp;
        for (int j = 0; j < 837; j++) {
            std::getline(pr, tmp);
            fout << tmp << std::endl;
        }
        for (int j = 837; j < 1336; j++) {
            std::getline(pr, tmp);
            size_t k = tmp.size();
            for (size_t i = 0; i < k; i++) {
                char t = tmp[i];
                if (t == 'C')
                    fout << C;
                else if (t == 'B')
                    fout << B;
                else
                    fout << t;
            }
            fout << std::endl;
        }
    }
    return 0;
}