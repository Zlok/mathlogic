#include <iostream>

void write_predic() {
    std::cout << "a=b->a=c->b=c" << std::endl;
    std::cout << "(a=b->a=c->b=c)->(A->A->A)->(a=b->a=c->b=c)" << std::endl;
    std::cout << "A->A->A" << std::endl;
    std::cout << "(A->A->A)->(a=b->a=c->b=c)" << std::endl;
    std::cout << "(A->A->A)->@a(a=b->a=c->b=c)" << std::endl;
    std::cout << "(A->A->A)->@b@a(a=b->a=c->b=c)" << std::endl;
    std::cout << "(A->A->A)->@c@b@a(a=b->a=c->b=c)" << std::endl;
    std::cout << "@c@b@a(a=b->a=c->b=c)" << std::endl;
    std::cout << "a+0=a" << std::endl;
    std::cout << "(a+0=a)->(A->A->A)->(a+0=a)" << std::endl;
    std::cout << "(A->A->A)->(a+0=a)" << std::endl;
    std::cout << "(A->A->A)->@a(a+0=a)" << std::endl;
    std::cout << "@a(a+0=a)" << std::endl;
    std::cout << "a=b->a\'=b\'" << std::endl;
    std::cout << "(a=b->a\'=b\')->(A->A->A)->(a=b->a\'=b\')" << std::endl;
    std::cout << "(A->A->A)->(a=b->a\'=b\')" << std::endl;
    std::cout << "(A->A->A)->@a(a=b->a\'=b\')" << std::endl;
    std::cout << "(A->A->A)->@b@a(a=b->a\'=b\')" << std::endl;
    std::cout << "@b@a(a=b->a\'=b\')" << std::endl;
    std::cout << "a+b\'=(a+b)\'" << std::endl;
    std::cout << "(a+b\'=(a+b)\')->(A->A->A)->(a+b\'=(a+b)\')" << std::endl;
    std::cout << "(A->A->A)->(a+b\'=(a+b)\')" << std::endl;
    std::cout << "(A->A->A)->@a(a+b\'=(a+b)\')" << std::endl;
    std::cout << "(A->A->A)->@b@a(a+b\'=(a+b)\')" << std::endl;
    std::cout << "@b@a(a+b\'=(a+b)\')" << std::endl; //25
}

void write_equal(std::string G) {
    G = "(" + G + ")";
    std::cout << "@c@b@a(a=b->a=c->b=c)->@b@a(a=b->a=" << G << "->b=" << G << ")" << std::endl;//45
    std::cout << "@b@a(a=b->a=" << G << "->b=" << G << ")" << std::endl;
    std::cout << "@b@a(a=b->a=" << G << "->b=" << G << ")->@a(a=" << G << "->a=" << G << "->" << G << "=" << G << ")" << std::endl;
    std::cout << "@a(a=" << G << "->a=" << G << "->" << G << "=" << G << ")" << std::endl;
    std::cout << "@a(a=" << G << "->a=" << G << "->" << G << "=" << G << ")->(" << G << "+0=" << G << "->" << G <<"+0=" << G << "->" << G << "=" << G << ")" << std::endl;
    std::cout << G << "+0=" << G << "->" << G <<"+0=" << G << "->" << G << "=" << G << std::endl;
    std::cout << "@a(a+0=a)->(" << G << "+0=" << G << ")" << std::endl;
    std::cout << G << "+0=" << G << std::endl;
    std::cout << G <<"+0=" << G << "->" << G << "=" << G << std::endl;
    std::cout << G << "=" << G << std::endl;
}

void add_one(std::string A, std::string K, std::string B) {
    A = "(" + A + ")";
    K = "(" + K + ")";
    B = "(" + B + ")";
    std::cout << "@b@a(a=b->a\'=b\')->@a(a=" << B << "->a\'=" << B << "\')" << std::endl;
    std::cout << "@a(a=" << B << "->a\'=" << B << "\')" << std::endl; //30
    std::cout << "@a(a=" << B << "->a\'=" << B << "\')->((" << A << "+" << K << ")=" << B << "->(" << A << "+" << K << ")\'=" << B << "\')" << std::endl;
    std::cout << "(" << A << "+" << K << ")=" << B << "->(" << A << "+" << K << ")\'=" << B << "\'" << std::endl;
    std::cout << "(" << A << "+" << K << ")\'=" << B << "\'" << std::endl;
    std::cout << "@b@a(a+b\'=(a+b)\')->@a(a+" << K << "\'=(a+" << K << ")\')" << std::endl;
    std::cout << "@a(a+" << K << "\'=(a+" << K << ")\')" << std::endl;//35
    std::cout << "@a(a+" << K << "\'=(a+" << K << ")\')->(" << A << "+" << K << "\'=(" << A << "+" << K << ")\')" << std::endl;
    std::cout << A << "+" << K << "\'=(" << A << "+" << K << ")\'" << std::endl;
    std::string a = "(" + A + "+" + K + "\')", b = "(" + A + "+" + K + ")\'";
    std::cout << "@c@b@a(a=b->a=c->b=c)->@b@a(a=b->a=" << a << "->b=" << a << ")" << std::endl;
    std::cout << "@b@a(a=b->a=" << a << "->b=" << a << ")" << std::endl;//40
    std::cout << "@b@a(a=b->a=" << a << "->b=" << a << ")->@a(a=" << b << "->a=" << a << "->" << b << "=" << a << ")" << std::endl;
    std::cout << "@a(a=" << b << "->a=" << a << "->" << b << "=" << a << ")" << std::endl;
    std::cout << "@a(a=" << b << "->a=" << a << "->" << b << "=" << a << ")->(" << a << "=" << b << "->" << a << "=" << a << "->" << b << "=" << a << ")" << std::endl;
    std::cout << a << "=" << b << "->" << a << "=" << a << "->" << b << "=" << a << std::endl;
    write_equal(a);
    std::cout << a << "=" << a << "->" << b << "=" << a << std::endl;
    std::cout << b << "=" << a << std::endl;
    a = b;
    b = "(" + A + "+" + K + "\')";
    std::string c = B + "\'";
    std::cout << "@c@b@a(a=b->a=c->b=c)->@b@a(a=b->a=" << c << "->b=" << c << ")" << std::endl;
    std::cout << "@b@a(a=b->a=" << c << "->b=" << c << ")" << std::endl;
    std::cout << "@b@a(a=b->a=" << c << "->b=" << c << ")->@a(a=" << b << "->a=" << c << "->" << b << "=" << c << ")" << std::endl;
    std::cout << "@a(a=" << b << "->a=" << c << "->" << b << "=" << c << ")" << std::endl;
    std::cout << "@a(a=" << b << "->a=" << c << "->" << b << "=" << c << ")->(" << a << "=" << b << "->" << a << "=" << c << "->" << b << "=" << c << ")" << std::endl;
    std::cout << a << "=" << b << "->" << a << "=" << c << "->" << b << "=" << c << std::endl;
    std::cout << a << "=" << c << "->" << b << "=" << c << std::endl;
    std::cout << b << "=" << c << std::endl;
}

void write_proof(std::string A, std::string B, std::string C, int c) {
    write_predic();
    std::string a = A, b = A, k = "0";
    std::cout << "@a(a+0=a)->(" << A << "+0=" << A << ")" << std::endl;
    std::cout << A << "+0=" << A << std::endl;//28
    for (int i = 0; i < c; i++) {
        add_one(a, k, b);
        b = b + "\'";
        k = k + "\'";
    }
    std::cout << "(((" << A << ")+(" << C << "))=(" << B << "))->?p(((" << A << ")+p)=(" << B << "))" << std::endl;
    std::cout << "?p(((" << A << ")+p)=(" << B << "))" << std::endl;
}

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    int a, b;
    std::cin >> a >> b;
    if (b >= a) {
        std::string A = "0", B = "0", C = "0";
        for (int i = 0; i < a; i++)
            A = A + "\'";
        for (int i = 0; i < b; i++)
            B = B + "\'";
        for (int i = 0; i < b - a; i++)
            C = C + "\'";
//        std::cout << A << " " << B << " " << C << std::endl;
        write_proof(A, B, C, b - a);
    } else {

    }
    return 0;
}