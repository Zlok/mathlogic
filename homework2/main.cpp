#include <iostream>
#include <stdio.h>
#include <vector>
#include <memory>
#include <map>
#include <fstream>

using namespace std;

ifstream fin("input.in");
ofstream fout("output.out");

struct expression {
    shared_ptr<expression> left, right;
    string value, op;
    vector< shared_ptr<expression> > terms;

    expression() : left(NULL), right(NULL), value(""), op(""), terms(NULL) {
    }
    expression(const expression& other) : left(other.left), right(other.right), value(other.value), op(other.op), terms(other.terms) {
    }
    ~expression() {
        if (left != NULL)
            left = NULL;
        if (right != NULL)
            right = NULL;
        if (terms.size()) {
            int tmp = (int)terms.size();
            for (int i = tmp - 1; i >= 0; i--)
                terms[i] = NULL;
        }
    }

    bool operator==(expression other) {
        return value == other.value;
    }

    bool has_variable(string val) {
        if (left == NULL && right == NULL)
            return value == val;
        if (right == NULL) {
            if (terms.size() != 0) {
                size_t tmp = terms.size();
                for (size_t i = 0; i < tmp; i++)
                    if (terms[i]->has_variable(val))
                        return true;
            }
            return left->has_variable(val);
        } else {
            return (left->has_variable(val) || right->has_variable(val));
        }
    }
    bool has_quant_for_value(string val) {
        if ((op == "?" || op == "@") && left->value == val)
            return true;
        if (left == NULL && right == NULL)
            return false;
        if (right == NULL)
            return left->has_quant_for_value(val);
        return (left->has_quant_for_value(val) || right->has_quant_for_value(val));
    }
};

struct implication : expression {
    implication(shared_ptr<expression> l, shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = "->";
        value = "(" + l->value + ")->(" + r->value + ")";
    }
};

struct disjunction : expression {
    disjunction(shared_ptr<expression> l, shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = "|";
        value = "(" + l->value + ")|(" + r->value + ")";
    }
};

struct conjunction : expression {
    conjunction(shared_ptr<expression> l, shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = "&";
        value = "(" + l->value + ")&(" + r->value + ")";
    }
};

struct negation : expression {
    negation(shared_ptr<expression> l) : expression() {
        left = l;
        op = "!";
        value = "!(" + l->value + ")";
    }
};

struct variable : expression {
    variable(string v) : expression() {
        value = v;
    }
};

struct quantifier : expression {
    quantifier(char c, shared_ptr<expression> l, shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = c;
        value = c + l->value + "(" + r->value + ")";
    }
};

struct name : expression {
    name(string v) : expression() {
        value = v;
    }
};

struct predication : expression {
    predication(shared_ptr<expression> l, vector< shared_ptr<expression> > t) : expression() {
        left = l;
        value = l->value;
        if (t.size() != 0) {
            terms = t;
            size_t tmp = t.size();
            value = value + "(";
            for (size_t i = 0; i < tmp - 1; i++)
                value = value + t[i]->value + ", ";
            value = value + t[tmp - 1]->value + ")";
        }
    }
};

struct equality : expression {
    equality(shared_ptr<expression> l, shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = "=";
        value = "(" + l->value + ")=(" + r->value + ")";
    }
};

struct addition : expression {
    addition(shared_ptr<expression> l, shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = "+";
        value = "(" + l->value + ")+(" + r->value + ")";
    }
};

struct multiplication : expression {
    multiplication(shared_ptr<expression> l, shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = "*";
        value = "(" + l->value + ")*(" + r->value + ")";
    }
};

struct next : expression {
    next(shared_ptr<expression> l) : expression() {
        left = l;
        op = "\'";
        value = "(" + l->value + ")\'";
    }
};

struct term : expression {
    term(shared_ptr<expression> l, vector< shared_ptr<expression> > t) : expression() {
        left = l;
        terms = t;
        value = l->value;
        if (t.size() != 0) {
            size_t tmp = t.size();
            value = value + "(";
            for (size_t i = 0; i < tmp - 1; i++)
                value = value + t[i]->value + ", ";
            value = value + t[tmp - 1]->value + ")";
        }
    }
};

struct zero : expression {
    zero() : expression() {
        value = "0";
    }
};

struct expression_parser {
    string value;
    int id;
    shared_ptr<expression> root;

    expression_parser() : value(), id(0), root(NULL) {
    }
    expression_parser(string& v) : value(v), id(0), root(NULL) {
        my_split();
        parse();
    }
    expression_parser(expression_parser const& other) : value(other.value), root(other.root) {
    }

    bool operator==(expression_parser other) {
        return (*root == *(other.root));
    }

    void parse() {
        root = parse_implication(parse_disjunction(parse_conjunction(parse_unary())));
    }

private:
    void my_split() {
        string tmp;
        size_t n = value.size();
        for (size_t i = 0; i < n; i++)
            if (value[i] != ' ')
                tmp.push_back(value[i]);
        value = tmp;
    }
    char get_char() {
        id++;
        return value[id - 1];
    }
    void return_char() {
        id--;
    }
    bool is_big_variable(char c) {
        return ('A' <= c && c <= 'Z');
    }
    string get_variable(char c) {
        string s;
        s.push_back(c);
        char c1 = get_char();
        while ('0' <= c1 && c1 <= '9') {
            s.push_back(c1);
            c1 = get_char();
        }
        return_char();
        return s;
    }

    shared_ptr<expression> parse_implication(shared_ptr<expression> left) {
        char c = get_char();
        if (c != '-') {
            return_char();
            return left;
        } else {
            c = get_char();
            shared_ptr<expression> tmp = shared_ptr<expression>(new implication(left, parse_implication(parse_disjunction(parse_conjunction(parse_unary())))));
            return tmp;
        }
    }
    shared_ptr<expression> parse_disjunction(shared_ptr<expression> left) {
        char c = get_char();
        if (c != '|') {
            return_char();
            return left;
        } else {
            shared_ptr<expression> right = parse_conjunction(parse_unary());
            return parse_disjunction(shared_ptr<expression>(new disjunction(left, right)));
        }
    }
    shared_ptr<expression> parse_conjunction(shared_ptr<expression> left) {
        char c = get_char();
        if (c != '&') {
            return_char();
            return left;
        } else {
            shared_ptr<expression> right = parse_unary();
            return parse_conjunction(shared_ptr<expression>(new conjunction(left, right)));
        }
    }
    shared_ptr<expression> parse_unary() {
        char c = get_char();
        if (c == '!')
            return shared_ptr<expression>(new negation(parse_unary()));
        if (c == '?' || c == '@')
            return shared_ptr<expression>(new quantifier(c, shared_ptr<expression>(new variable(get_variable(get_char()))), parse_unary()));
        if (c == '(') {
            shared_ptr<expression> ans = parse_implication(parse_disjunction(parse_conjunction(parse_unary())));
            c = get_char();
            return parse_term(parse_addition(parse_multiplication(parse_value(ans))));
        }
        return_char();
        return parse_predicate();
    }

    shared_ptr<expression> parse_predicate() {
        char c = get_char();
        if (is_big_variable(c)) {
            string s = get_variable(c);
            c = get_char();
            if (c != '(') {
                return_char();
                vector< shared_ptr<expression> > tmp(NULL);
                return shared_ptr<expression>(new predication(shared_ptr<expression>(new name(s)), tmp));
            } else {
                vector< shared_ptr<expression> > tmp;
                while (c != ')') {
                    tmp.push_back(parse_term(parse_addition(parse_multiplication(parse_value()))));
                    c = get_char();
                }
                return shared_ptr<expression>(new predication(shared_ptr<expression>(new name(s)), tmp));
            }
        } else if (c == '(') {
            shared_ptr<expression> tmp = parse_implication(parse_disjunction(parse_conjunction(parse_unary())));
            c = get_char();
            return tmp;
        } else {
            return_char();
            return parse_term(parse_addition(parse_multiplication(parse_value())));
        }
    }
    shared_ptr<expression> parse_term(shared_ptr<expression> left) {
        char c = get_char();
        if (c != '=') {
            return_char();
            return left;
        } else {
            shared_ptr<expression> right = parse_addition(parse_multiplication(parse_value()));
            return shared_ptr<expression>(new equality(left, right));
        }
    }
    shared_ptr<expression> parse_addition(shared_ptr<expression> left) {
        char c = get_char();
        if (c != '+') {
            return_char();
            return left;
        } else {
            shared_ptr<expression> right = parse_multiplication(parse_value());
            return parse_addition(shared_ptr<expression>(new addition(left, right)));
        }
    }
    shared_ptr<expression> parse_multiplication(shared_ptr<expression> left) {
        char c = get_char();
        if (c != '*') {
            return_char();
            return left;
        } else {
            shared_ptr<expression> right = parse_value();
            return parse_multiplication(shared_ptr<expression>(new multiplication(left, right)));
        }
    }
    shared_ptr<expression> parse_value() {
        char c = get_char();
        if (c == '0') {
            return parse_value(shared_ptr<expression>(new zero()));
        } else if (c != '(') {
            string s = get_variable(c);
            c = get_char();
            if (c == '(') {
                vector< shared_ptr<expression> > tmp;
                while (c != ')') {
                    tmp.push_back(parse_term(parse_addition(parse_multiplication(parse_value()))));
                    c = get_char();
                }
                return parse_value(shared_ptr<expression>(new term(shared_ptr<expression>(new name(s)), tmp)));
            } else {
                return_char();
                return parse_value(shared_ptr<expression>(new variable(s)));
            }
        } else {
            shared_ptr<expression> ans = parse_term(parse_addition(parse_multiplication(parse_value())));
            c = get_char();
            return parse_value(ans);
        }
    }
    shared_ptr<expression> parse_value(shared_ptr<expression> left) {
        char c = get_char();
        if (c != '\'') {
            return_char();
            return left;
        } else {
            shared_ptr<expression> t = shared_ptr<expression>(new next(left));
            return parse_value(t);
        }
    }
};

struct global_parser {
    vector<expression_parser> axioms;
    vector<expression_parser> assumptions;
    vector<expression_parser> proof;
    vector<string> end_proof;
    expression_parser final;

    global_parser() {
        add_axioms();
        parse_assump_and_final();
        parse_proof();
    }

    void add_axioms() {
        string s = "a->b->a";
        axioms.push_back(expression_parser(s)); //1
        s = "(a->b)->(a->b->c)->(a->c)";
        axioms.push_back(expression_parser(s)); //2
        s = "a->b->a&b";
        axioms.push_back(expression_parser(s)); //3
        s = "a&b->a";
        axioms.push_back(expression_parser(s)); //4
        s = "a&b->b";
        axioms.push_back(expression_parser(s)); //5
        s = "a->a|b";
        axioms.push_back(expression_parser(s)); //6
        s = "b->a|b";
        axioms.push_back(expression_parser(s)); //7
        s = "(a->b)->(c->b)->(a|c->b)";
        axioms.push_back(expression_parser(s)); //8
        s = "(a->b)->(a->!b)->!a";
        axioms.push_back(expression_parser(s)); //9
        s = "!!a->a";
        axioms.push_back(expression_parser(s)); //10

        s = "a=b->a\'=b\'";
        axioms.push_back(expression_parser(s)); //1
        s = "a=b->a=c->b=c";
        axioms.push_back(expression_parser(s)); //2
        s = "a\'=b\'->a=b";
        axioms.push_back(expression_parser(s)); //3
        s = "!(a\'=0)";
        axioms.push_back(expression_parser(s)); //4
        s = "a+b\'=(a+b)\'";
        axioms.push_back(expression_parser(s)); //5
        s = "a+0=a";
        axioms.push_back(expression_parser(s)); //6
        s = "a*0=0";
        axioms.push_back(expression_parser(s)); //7
        s = "a*b\'=a*b+a";
        axioms.push_back(expression_parser(s)); //8
        //axioms.push_back(expression_parser("@xA(x)->A(q)")); // q свободно в А
        //axioms.push_back(expression_parser("A(q)->?xA(x)")); // q свободно в А
        //axioms.push_back(expression_parser("A(0)&@x(A(x)->A(x'))->A(x)")); // x свободно в А
        //for (size_t i = 0; i < 18; i++)
        //  axioms[i].root->draw();
    }

    void parse_assump_and_final() {
        string s;
        getline(fin, s);
        s = my_split(s);
        string tmp = "";
        size_t n = s.size(), i = 0, balance = 0;
        while (i < n) {
            if (s[i] == '|' && s[i + 1] == '-') {
                i += 2;
                if (tmp != "")
                    assumptions.push_back(expression_parser(tmp));
                tmp = "";
            } else if (balance == 0 && s[i] == ',') {
                i++;
                assumptions.push_back(expression_parser(tmp));
                tmp = "";
            } else {
                tmp.push_back(s[i]);
                if (s[i] == '(')
                    balance++;
                else if (s[i] == ')')
                    balance--;
                i++;
            }
        }
        final = expression_parser(tmp);
    }

    void parse_proof() {
        string s;
        while (getline(fin, s)) {
            s = my_split(s);
            if (s != "") {
                proof.push_back(expression_parser(s));
                pair<bool, string> tmp = check();
                if (!tmp.first) {
                    fout << "Вывод некорректен начиная с формулы номер " << proof.size() << " " << tmp.second << endl;
                    return;
                }
            }
        }

        if (proof[proof.size() - 1] == final) {
            size_t tmp = end_proof.size();
            for (size_t i = 0; i < tmp; i++)
                cout << end_proof[i] << endl;
        } else {
            cout << "Доказательство не окончено..." << endl;
        }
    }

    pair<bool, string> check() {
        if (is_assump())
            return make_pair(true, "");
        if (is_axiom())
            return make_pair(true, "");
        if (is_MP())
            return make_pair(true, "");
        pair<bool, string> miy = is_rule();
        if (miy.first || miy.second != "")
            return miy;
        miy = is_axiom_for_free();
        if (miy.first || miy.second != "")
            return miy;
        return make_pair(false, "");
    }

    bool is_assump() {
        size_t tmp = assumptions.size();
        if (tmp == 0)
            return false;
        expression_parser t = proof[proof.size() - 1];
        for (size_t i = 0; i < tmp - 1; i++) {
            if (t == assumptions[i]) {
                end_proof.push_back(t.value);
                end_proof.push_back("(" + t.value + ")->(" + assumptions[tmp - 1].value + ")->(" + t.value + ")");
                end_proof.push_back("(" + assumptions[tmp - 1].value + ")->(" + t.value + ")");
                return true;
            }
        }
        if (t == assumptions[tmp]) {
            end_proof.push_back("(" + t.value + ")->(" + t.value + ")->(" + t.value + ")");
            end_proof.push_back("(" + t.value + ")->((" + t.value + ")->(" + t.value + "))->(" + t.value + ")");
            end_proof.push_back("((" + t.value + ")->(" + t.value + ")->(" + t.value + "))->(("
                                + t.value + ")->((" + t.value + ")->(" + t.value + "))->(" + t.value + "))->(("
                                + t.value + ")->(" + t.value + "))");
            end_proof.push_back("((" + t.value + ")->((" + t.value + ")->(" + t.value + "))->(" + t.value + "))->(("
                                + t.value + ")->(" + t.value + "))");
            end_proof.push_back("(" + t.value + ")->(" + t.value + ")");
            return true;
        }
        return false;
    }
    bool is_axiom() {
        expression_parser t = proof[proof.size() - 1];
        for (size_t i = 0; i < 10; i++)
            if (equal_axiom(t, axioms[i])) {
                if (assumptions.size() == 0) {
                    end_proof.push_back(t.value);
                    return true;
                }
                end_proof.push_back(t.value);
                end_proof.push_back("(" + t.value + ")->(" + assumptions[assumptions.size() - 1].value + ")->(" + t.value + ")");
                end_proof.push_back("(" + assumptions[assumptions.size() - 1].value + ")->(" + t.value + ")");
                return true;
            }
        for (size_t i = 10; i < 18; i++)
            if (t == axioms[i]) {
                if (assumptions.size() == 0) {
                    end_proof.push_back(t.value);
                    return true;
                }
                end_proof.push_back(t.value);
                end_proof.push_back("(" + t.value + ")->(" + assumptions[assumptions.size() - 1].value + ")->(" + t.value + ")");
                end_proof.push_back("(" + assumptions[assumptions.size() - 1].value + ")->(" + t.value + ")");
                return true;
            }
        return false;
    }
    bool equal_axiom(expression_parser ex, expression_parser ax) {
        map<string, string> match;
        return equal_axiom(ex.root, ax.root, match);
    }
    bool equal_axiom(shared_ptr<expression> ex, shared_ptr<expression> ax, map<string, string>& match) {
        if (ax->left == NULL && ax->right == NULL) {
            if (match.count(ax->value))
                return match[ax->value] == ex->value;
            match.insert(make_pair(ax->value, ex->value));
            return true;
        } else {
            if (ex->op == ax->op) {
                if (ex->left == NULL || equal_axiom(ex->left, ax->left, match))
                if (ex->right == NULL || equal_axiom(ex->right, ax->right, match)) {
                    if (ex->terms.size() == 0 && ex->terms.size() == ax->terms.size())
                        return true;
                    else if (ex->terms.size() == ax->terms.size()) {
                        size_t tmp = ex->terms.size();
                        for (size_t i = 0; i < tmp; i++)
                            if (!equal_axiom(ex->terms[i], ax->terms[i], match))
                                return false;
                        return true;
                    }
                }
            }
            return false;
        }
    }
    bool is_MP() {
        expression_parser t = proof[proof.size() - 1];
        size_t tmp = proof.size();
        for (size_t i = 0; i < tmp - 1; i++)
            if (proof[i].root->op == "->" && *(proof[i].root->right) == *(t.root))
                for (size_t j = 0; j < tmp - 1; j++)
                    if (*(proof[j].root) == *(proof[i].root->left)) {
                        if (assumptions.size() == 0)
                            end_proof.push_back(t.value);
                        else {
                            end_proof.push_back("(("+assumptions[assumptions.size() - 1].value
                                                +")->("+proof[j].value+")"
                                                +")->(("+assumptions[assumptions.size() - 1].value+")->("
                                                +proof[i].value+"))->(("
                                                +assumptions[assumptions.size() - 1].value+")->("
                                                +t.value+"))");
                            end_proof.push_back("(("+assumptions[assumptions.size() - 1].value+")->("
                                                +proof[i].value+"))->(("
                                                +assumptions[assumptions.size() - 1].value+")->("
                                                +t.value+"))");
                            end_proof.push_back("((" +assumptions[assumptions.size() - 1].value
                                                +")->(" +t.value+"))");
                        }
                        return true;
                    }
        return false;
    }
    pair<bool, string> is_rule() {
        expression_parser t = proof[proof.size() - 1];
        if (t.root->op == "->") {
            if (t.root->right->op == "@") {
                size_t tmp = proof.size();
                for (size_t i = 0; i < tmp - 1; i++) {
                    if (proof[i].root->op == "->" && *(proof[i].root->left) == *(t.root->left) && *(proof[i].root->right) == *(t.root->right->right)) {
                        int f = check_free(t.root->left, t.root->right->left->value);
                        if (f == 1 || f == 3)
                            return make_pair(false, ": переменная " +
                                                         t.root->right->left->value + " входит свободно в формулу " +
                                                         t.root->left->value + ".");
                        if (assumptions.size() == 0)
                            end_proof.push_back(t.value);
                        else
                            add_to_end_proof(i);
                        return make_pair(true, "");
                    }
                }
            }
            if (t.root->left->op == "?") {
                size_t tmp = proof.size();
                for (size_t i = 0; i < tmp - 1; i++) {
                    if (proof[i].root->op == "->" && *(proof[i].root->left) == *(t.root->left->right) && *(proof[i].root->right) == *(t.root->right)) {
                        int f = check_free(t.root->right, t.root->left->left->value);
                        if (f == 1 || f == 3)
                            return make_pair(false, ": переменная " +
                                                         t.root->left->left->value + " входит свободно в формулу " +
                                                         t.root->right->value + ".");
                        if (assumptions.size() == 0)
                            end_proof.push_back(t.value);
                        else
                            add_to_end_proof(i);
                        return make_pair(true, "");
                    }
                }
            }
        }
        return make_pair(false, "");
    }
    int check_free(shared_ptr<expression> ex, string val) {
        if (!ex->has_variable(val))
            return 2;
        if ((ex->op == "@" || ex->op == "?") && ex->left->value == val)
            return 0;
        if (ex->right == NULL) {
            if (ex->terms.size() != 0) {
                size_t tmp = ex->terms.size();
                for (size_t i = 0; i < tmp; i++)
                    if (ex->terms[i]->has_variable(val))
                        return 1;
            }
            return ex->left->has_variable(val);
        }
        int fl = check_free(ex->left, val), fr = check_free(ex->right, val);
        switch (fl) {
            case 0:
                switch (fr) {
                    case 0:
                        return 0;
                    case 1:
                        return 3;
                    case 2:
                        return 0;
                    case 3:
                        return 3;
                }
            case 1:
                switch (fr) {
                    case 0:
                        return 3;
                    case 1:
                        return 1;
                    case 2:
                        return 1;
                    case 3:
                        return 3;
                }
            case 2:
                return fr;
            case 3:
                return 3;
        }
    }
    pair<bool, string> is_axiom_for_free() {
        expression_parser t = proof[proof.size() - 1];
        if (t.root->op == "->") {
            if (t.root->left->op == "@") {
                map<string, string> match;
                if (equal_axiom(t.root->right, t.root->left->right, match)) {
                    map<string, string>::iterator it = match.begin();
                    string q;
                    bool f = true;
                    for (; it != match.end(); it++)
                        if (it->first == t.root->left->left->value) {
                            q = it->second;
                        } else if (it->first != it->second) {
                            f = false;
                            break;
                        }
                    if (f) {
                        if (!check_free(t.root->left->right, q, t.root->left->left->value))
                            return make_pair(false, ": терм " + q + " не свободен для подстановки в формулу " +
                                                         t.root->left->right->value + " вместо переменной " +
                                                         t.root->left->left->value + ".");
                        if (assumptions.size() == 0) {
                            end_proof.push_back(t.value);
                            return make_pair(true, "");
                        }
                        end_proof.push_back(t.value);
                        end_proof.push_back(
                                "(" + t.value + ")->(" + assumptions[assumptions.size() - 1].value + ")->(" + t.value +
                                ")");
                        end_proof.push_back("(" + assumptions[assumptions.size() - 1].value + ")->(" + t.value + ")");
                        return make_pair(true, "");
                    }
                }
            }

            if (t.root->right->op == "?") {
                map<string, string> match;
                if (equal_axiom(t.root->left, t.root->right->right, match)) {
                    map<string, string>::iterator it = match.begin();
                    string q;
                    bool f = true;
                    for (; it != match.end(); it++)
                        if (it->first == t.root->right->left->value) {
                            q = it->second;
                        } else if (it->first != it->second) {
                            f = false;
                            break;
                        }
                    if (f) {
                        if (!check_free(t.root->right->right, q, t.root->right->left->value))
                            return make_pair(false, ": терм " + q + " не свободен для подстановки в формулу " +
                                                         t.root->right->right->value + " вместо переменной " +
                                                         t.root->right->left->value + ".");
                        if (assumptions.size() == 0) {
                            end_proof.push_back(t.value);
                            return make_pair(true, "");
                        }
                        end_proof.push_back(t.value);
                        end_proof.push_back(
                                "(" + t.value + ")->(" + assumptions[assumptions.size() - 1].value + ")->(" + t.value +
                                ")");
                        end_proof.push_back("(" + assumptions[assumptions.size() - 1].value + ")->(" + t.value + ")");
                        return make_pair(true, "");
                    }
                }
            }

            if (t.root->left->op == "&" && t.root->left->right->op == "@" && t.root->left->right->right->op == "->") {
                map<string, string> match;
                if (equal_axiom(t.root->left->right->right->right, t.root->left->right->right->left, match)) {
                    map<string, string>::iterator it = match.begin();
                    string q;
                    bool f = true;
                    for (; it != match.end(); it++)
                        if (it->first == t.root->left->right->left->value) {
                            q = it->second;
                        } else if (it->first != it->second) {
                            f = false;
                            break;
                        }
                    if (f && q == "(" + t.root->left->right->left->value + ")\'") {
                        match.clear();
                        if (equal_axiom(t.root->left->left, t.root->left->right->right->left, match)) {
                            it = match.begin();
                            f = true;
                            for (; it != match.end(); it++)
                                if (it->first == t.root->left->right->left->value) {
                                    q = it->second;
                                } else if (it->first != it->second) {
                                    f = false;
                                    break;
                                }
                            if (f && q == "0") {
                                if (*(t.root->left->right->right->left) == *(t.root->right)) {
                                    if (assumptions.size() == 0) {
                                        end_proof.push_back(t.value);
                                        return make_pair(true, "");
                                    }
                                    end_proof.push_back(t.value);
                                    end_proof.push_back(
                                            "(" + t.value + ")->(" + assumptions[assumptions.size() - 1].value +
                                            ")->(" +
                                            t.value +
                                            ")");
                                    end_proof.push_back(
                                            "(" + assumptions[assumptions.size() - 1].value + ")->(" + t.value + ")");
                                    return make_pair(true, "");
                                }
                            }
                        }
                    }
                }
            }
        }
        return make_pair(false, "");
    }
    bool check_free(shared_ptr<expression> ex, string ter, string val) {
        if ((ex->op == "@" || ex->op == "?") && ex->right->has_variable(val)) {
            vector<string> value_in_ter = get_values(ter);
            size_t n = value_in_ter.size();
            for (size_t i = 0; i < n; i++)
                if (ex->left->value == value_in_ter[i])
                    return false;
        }
        if (ex->left != NULL && !check_free(ex->left, ter, val))
            return false;
        if (ex->right != NULL && !check_free(ex->right, ter, val))
            return false;
        return true;
    }
private:
    string my_split(string t) {
        string tmp;
        size_t n = t.size();
        for (size_t i = 0; i < n; i++)
            if (t[i] != ' ')
                tmp.push_back(t[i]);
        return tmp;
    }
    global_parser(string& a) {
        add_axioms();
        parse_add_assump_and_final(a);
    }
    void parse_add_assump_and_final(string& s) {
        s = my_split(s);
        string tmp = "";
        size_t n = s.size(), i = 0, balance = 0;
        while (i < n) {
            if (s[i] == '|' && s[i + 1] == '-') {
                i += 2;
                if (tmp != "")
                    assumptions.push_back(expression_parser(tmp));
                tmp = "";
            } else if (balance == 0 && s[i] == ',') {
                i++;
                assumptions.push_back(expression_parser(tmp));
                tmp = "";
            } else {
                tmp.push_back(s[i]);
                if (s[i] == '(')
                    balance++;
                else if (s[i] == ')')
                    balance--;
                i++;
            }
        }
        final = expression_parser(tmp);
    }
    vector<string> parse_add_proof(vector<string> t) {
        size_t tmp = t.size();
        string s;
        for (size_t i = 0; i < tmp; i++) {
            s = t[i];
            if (s != "") {
                proof.push_back(expression_parser(s));
                pair<bool, string> miy = check();
                if (!miy.first) {
                    break;
                }
            }
        }
        if (proof[proof.size() - 1] == final) {
            return end_proof;
        }
    }
    void add_to_end_proof(size_t id) {
        expression_parser t = proof[proof.size() - 1], a = assumptions[assumptions.size() - 1];
        if (t.root->right->op == "@") {
            string ass;
            shared_ptr<expression> b = proof[id].root->left, g = proof[id].root->right;
            ass = "(" + a.value + ")->(" + proof[id].root->value + "),(" + a.value +")&(" + b->value + ")|-" + g->value;
            global_parser tmp(ass);
            vector<string> pr;
            pr.push_back("((" + a.value +")&(" + b->value + "))->(" + a.value + ")");
            pr.push_back("((" + a.value +")&(" + b->value + "))->(" + b->value + ")");
            pr.push_back("(" + a.value + ")&(" + b->value + ")");
            pr.push_back(a.value);
            pr.push_back(b->value);
            pr.push_back("(" + a.value + ")->(" + proof[id].root->value + ")");
            pr.push_back(proof[id].root->value);
            pr.push_back(g->value);
            pr = tmp.parse_add_proof(pr);
            ass = "(" + a.value + ")->(" + proof[id].root->value + ")|-((" + a.value + ")&(" + b->value + "))->(" + g->value + ")";
            global_parser tmp2(ass);
            pr = tmp2.parse_add_proof(pr);
            size_t n = pr.size();
            for (size_t i = 0; i < n; i++)
                end_proof.push_back(pr[i]);
            end_proof.push_back("((" + a.value + ")&(" + b->value + "))->(" + g->value + ")");
            g = t.root->right;
            end_proof.push_back("((" + a.value + ")&(" + b->value + "))->(" + g->value + ")");
            ass = "((" + a.value + ")&(" + b->value + "))->(" + g->value + ")," + a.value + "," + b->value + "|-" + g->value;
            global_parser tmp3(ass);
            vector<string> pr2;
            pr2.push_back("(" + a.value + ")->(" + b->value + ")->((" + a.value + ")&(" + b->value + "))");
            pr2.push_back(a.value);
            pr2.push_back(b->value);
            pr2.push_back("(" + b->value + ")->((" + a.value + ")&(" + b->value + "))");
            pr2.push_back("(" + a.value + ")&(" + b->value + ")");
            pr2.push_back("((" + a.value + ")&(" + b->value + "))->(" + g->value + ")");
            pr2.push_back(g->value);
            pr2 = tmp3.parse_add_proof(pr2);
            ass = "((" + a.value + ")&(" + b->value + "))->(" + g->value + ")," + a.value + "|-(" + b->value + ")->(" + g->value + ")";
            global_parser tmp4(ass);
            pr2 = tmp4.parse_add_proof(pr2);
            ass = "((" + a.value + ")&(" + b->value + "))->(" + g->value + ")""|-(" + a.value + ")->(" + t.value + ")";
            global_parser tmp5(ass);
            pr2 = tmp5.parse_add_proof(pr2);
            n = pr2.size();
            for (size_t i = 0; i < n; i++)
                end_proof.push_back(pr2[i]);
            end_proof.push_back("(" + a.value + ")->(" + t.value + ")");
        } else if (t.root->left->op == "?") {
            string ass;
            shared_ptr<expression> b = proof[id].root->left, g = proof[id].root->right;
            ass = "(" + a.value + ")->(" + b->value + ")->(" + g->value + ")," + b->value + "," + a.value + "|-" + g->value;
            global_parser tmp(ass);
            vector<string> pr;
            pr.push_back("(" + a.value + ")->(" + proof[id].value + ")");
            pr.push_back(a.value);
            pr.push_back(b->value);
            pr.push_back(proof[id].value);
            pr.push_back(g->value);
            pr = tmp.parse_add_proof(pr);
            ass = "(" + a.value + ")->(" + b->value + ")->(" + g->value + ")," + b->value + "|-(" + a.value + ")->(" + g->value + ")";
            global_parser tmp2(ass);
            pr = tmp2.parse_add_proof(pr);
            ass = "(" + a.value + ")->(" + b->value + ")->(" + g->value + ")" + "|-(" + b->value + ")->(" + a.value + ")->(" + g->value + ")";
            global_parser tmp3(ass);
            pr = tmp3.parse_add_proof(pr);
            size_t n = pr.size();
            for (size_t i = 0; i < n; i++)
                end_proof.push_back(pr[i]);
            end_proof.push_back("(" + b->value + ")->(" + a.value + ")->(" + g->value + ")");
            b = t.root->left;
            end_proof.push_back("(" + b->value + ")->(" + a.value + ")->(" + g->value + ")");
            ass = "(" + b->value + ")->(" + a.value + ")->(" + g->value + ")," + a.value + "," + b->value + "|-" + g->value;
            global_parser tmp4(ass);
            vector<string> pr2;
            pr2.push_back("(" + b->value + ")->(" + a.value + ")->(" + g->value + ")");
            pr2.push_back(a.value);
            pr2.push_back(b->value);
            pr2.push_back("(" + a.value + ")->(" + g->value + ")");
            pr2.push_back(g->value);
            pr2 = tmp4.parse_add_proof(pr2);
            ass = "(" + b->value + ")->(" + a.value + ")->(" + g->value + ")," + a.value + "|-(" + b->value + ")->(" + g->value + ")";
            global_parser tmp5(ass);
            pr2 = tmp5.parse_add_proof(pr2);
            ass = "(" + b->value + ")->(" + a.value + ")->(" + g->value + ")" + "|-(" + a.value + ")->(" + b->value + ")->(" + g->value + ")";
            global_parser tmp6(ass);
            pr2 = tmp6.parse_add_proof(pr2);
            n = pr2.size();
            for (size_t i = 0; i < n; i++)
                end_proof.push_back(pr2[i]);
            end_proof.push_back("(" + a.value + ")->(" + t.value + ")");
        }
    }
    vector<string> get_values(string t) {
        size_t tmp = t.size(), i = 0;
        string q;
        vector<string> ans;
        while (i < tmp) {
            if ('a' <= t[i] && t[i] <= 'z') {
                q = q + t[i];
                i++;
                while ('0' <= t[i] && t[i] <= '9') {
                    q = q + t[i];
                    i++;
                }
                ans.push_back(q);
                q = "";
            } else
                i++;
        }
        return ans;
    }
};

int main() {
    global_parser tmp;
    return 0;
}
