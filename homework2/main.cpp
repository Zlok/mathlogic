#include <iostream>
#include <stdio.h>
#include <vector>
#include <memory>
#include <map>

struct expression {
    std::shared_ptr<expression> left, right;
    std::string value, op;
    std::vector< std::shared_ptr<expression> > terms;
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

    void draw() {
        std::cout << value << std::endl;
    }

    bool has_variable(std::string v) {
        if (op == "" && left == NULL)
            return value == v;
        if (terms.size() != 0) {
            size_t tmp = terms.size();
            for (size_t i = 0; i < tmp; i++)
                if (terms[i]->has_variable(v))
                    return true;
        }
        if (right != NULL && right->has_variable(v))
            return true;
        if (left != NULL && left->has_variable(v))
            return true;
        return false;
    }
    bool has_quantif_for_variable(std::string v) {
        if (op == "")
            return false;
        if (op == "?" || op == "@")
            if (left->value == v)
                return true;
        if (right == NULL || !right->has_quantif_for_variable(v))
            return (left != NULL && left->has_quantif_for_variable(v));
        return true;
    }
    void get_variable(std::vector<std::string> & str) {
        if (op == "" && 'a' <= value[0] && value[0] <= 'z' && terms.size() == 0)
            str.push_back(value);
        if (left != NULL)
            left->get_variable(str);
        if (right != NULL)
            right->get_variable(str);
        if (terms.size() != 0) {
            size_t tmp = terms.size();
            for (size_t i = 0; i < tmp; i++)
                terms[i]->get_variable(str);
        }
    }
    void get_free_variable(std::vector<std::string> & str) {
        std::vector<std::string> tmp;
        this->get_variable(tmp);
        size_t n = tmp.size();
        if (n == 0)
            return;
        for (size_t i = 0; i < n; i++)
            if (!(this->has_quantif_for_variable(tmp[i])))
                str.push_back(tmp[i]);
    }
};

struct implication : expression {
    implication(std::shared_ptr<expression> l, std::shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = "->";
        value = "(" + l->value + ")->(" + r->value + ")";
    }
};

struct disjunction : expression {
    disjunction(std::shared_ptr<expression> l, std::shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = "|";
        value = "(" + l->value + ")|(" + r->value + ")";
    }
};

struct conjunction : expression {
   conjunction(std::shared_ptr<expression> l, std::shared_ptr<expression> r) : expression() {
       left = l;
       right = r;
       op = "&";
       value = "(" + l->value + ")&(" + r->value + ")";
    }
};

struct negation : expression {
    negation(std::shared_ptr<expression> l) : expression() {
        left = l;
        op = "!";
        value = "!(" + l->value + ")";
    }
};

struct variable : expression {
    variable(std::string v) : expression() {
        value = v;
    }
};

struct quantifier : expression {
    quantifier(char c, std::shared_ptr<expression> l, std::shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = c;
        value = c + l->value + "(" + r->value + ")";
    }
};

struct name : expression {
    name(std::string v) : expression() {
        value = v;
    }
};

struct predication : expression {
    predication(std::shared_ptr<expression> l, std::vector< std::shared_ptr<expression> > t) : expression() {
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
    equality(std::shared_ptr<expression> l, std::shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = "=";
        value = "(" + l->value + ")=(" + r->value + ")";
    }
};

struct addition : expression {
    addition(std::shared_ptr<expression> l, std::shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = "+";
        value = "(" + l->value + ")+(" + r->value + ")";
    }
};

struct multiplication : expression {
    multiplication(std::shared_ptr<expression> l, std::shared_ptr<expression> r) : expression() {
        left = l;
        right = r;
        op = "*";
        value = "(" + l->value + ")*(" + r->value + ")";
    }
};

struct next : expression {
    next(std::shared_ptr<expression> l) : expression() {
        left = l;
        op = "\'";
        value = "(" + l->value + ")\'";
    }
};

struct term : expression {
    term(std::shared_ptr<expression> l, std::vector< std::shared_ptr<expression> > t) : expression() {
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
    std::string value;
    int id;
    std::shared_ptr<expression> root;

    expression_parser() : value(), id(0), root(NULL) {
    }
    expression_parser(std::string& v) : value(v), id(0), root(NULL) {
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
        std::string tmp;
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
    std::string get_variable(char c) {
        std::string s;
        s.push_back(c);
        char c1 = get_char();
        while ('0' <= c1 && c1 <= '9') {
            s.push_back(c1);
            c1 = get_char();
        }
        return_char();
        return s;
    }

    std::shared_ptr<expression> parse_implication(std::shared_ptr<expression> left) {
        char c = get_char();
        if (c != '-') {
            return_char();
            return left;
        } else {
            c = get_char();
            std::shared_ptr<expression> tmp = std::shared_ptr<expression>(new implication(left, parse_implication(parse_disjunction(parse_conjunction(parse_unary())))));
            return tmp;
        }
    }
    std::shared_ptr<expression> parse_disjunction(std::shared_ptr<expression> left) {
        char c = get_char();
        if (c != '|') {
            return_char();
            return left;
        } else {
            std::shared_ptr<expression> right = parse_conjunction(parse_unary());
            return parse_disjunction(std::shared_ptr<expression>(new disjunction(left, right)));
        }
    }
    std::shared_ptr<expression> parse_conjunction(std::shared_ptr<expression> left) {
        char c = get_char();
        if (c != '&') {
            return_char();
            return left;
        } else {
            std::shared_ptr<expression> right = parse_unary();
            return parse_conjunction(std::shared_ptr<expression>(new conjunction(left, right)));
        }
    }
    std::shared_ptr<expression> parse_unary() {
        char c = get_char();
        if (c == '!')
            return std::shared_ptr<expression>(new negation(parse_unary()));
        if (c == '?' || c == '@')
            return std::shared_ptr<expression>(new quantifier(c, std::shared_ptr<expression>(new variable(get_variable(get_char()))), parse_unary()));
        if (c == '(') {
            std::shared_ptr<expression> ans = parse_implication(parse_disjunction(parse_conjunction(parse_unary())));
            c = get_char();
            return parse_term(parse_addition(parse_multiplication(parse_value(ans))));
        }
        return_char();
        return parse_predicate();
    }

    std::shared_ptr<expression> parse_predicate() {
        char c = get_char();
        if (is_big_variable(c)) {
            std::string s = get_variable(c);
            c = get_char();
            if (c != '(') {
                return_char();
                std::vector< std::shared_ptr<expression> > tmp(NULL);
                return std::shared_ptr<expression>(new predication(std::shared_ptr<expression>(new name(s)), tmp));
            } else {
                std::vector< std::shared_ptr<expression> > tmp;
                while (c != ')') {
                    tmp.push_back(parse_term(parse_addition(parse_multiplication(parse_value()))));
                    c = get_char();
                }
                return std::shared_ptr<expression>(new predication(std::shared_ptr<expression>(new name(s)), tmp));
            }
        } else if (c == '(') {
            std::shared_ptr<expression> tmp = parse_implication(parse_disjunction(parse_conjunction(parse_unary())));
            c = get_char();
            return tmp;
        } else {
            return_char();
            return parse_term(parse_addition(parse_multiplication(parse_value())));
        }
    }
    std::shared_ptr<expression> parse_term(std::shared_ptr<expression> left) {
        char c = get_char();
        if (c != '=') {
            return_char();
            return left;
        } else {
            std::shared_ptr<expression> right = parse_addition(parse_multiplication(parse_value()));
            return std::shared_ptr<expression>(new equality(left, right));
        }
    }
    std::shared_ptr<expression> parse_addition(std::shared_ptr<expression> left) {
        char c = get_char();
        if (c != '+') {
            return_char();
            return left;
        } else {
            std::shared_ptr<expression> right = parse_multiplication(parse_value());
            return parse_addition(std::shared_ptr<expression>(new addition(left, right)));
        }
    }
    std::shared_ptr<expression> parse_multiplication(std::shared_ptr<expression> left) {
        char c = get_char();
        if (c != '*') {
            return_char();
            return left;
        } else {
            std::shared_ptr<expression> right = parse_value();
            return parse_multiplication(std::shared_ptr<expression>(new multiplication(left, right)));
        }
    }
    std::shared_ptr<expression> parse_value() {
        char c = get_char();
        if (c == '0') {
            return parse_value(std::shared_ptr<expression>(new zero()));
        } else if (c != '(') {
            std::string s = get_variable(c);
            c = get_char();
            if (c == '(') {
                std::vector< std::shared_ptr<expression> > tmp;
                while (c != ')') {
                    tmp.push_back(parse_term(parse_addition(parse_multiplication(parse_value()))));
                    c = get_char();
                }
                return parse_value(std::shared_ptr<expression>(new term(std::shared_ptr<expression>(new name(s)), tmp)));
            } else {
                return_char();
                return parse_value(std::shared_ptr<expression>(new variable(s)));
            }
        } else {
            std::shared_ptr<expression> ans = parse_term(parse_addition(parse_multiplication(parse_value())));
            c = get_char();
            return parse_value(ans);
        }
    }
    std::shared_ptr<expression> parse_value(std::shared_ptr<expression> left) {
        char c = get_char();
        if (c != '\'') {
            return_char();
            return left;
        } else {
            return parse_value(std::shared_ptr<expression>(new next(left)));
        }
    }
};

struct global_parser {
    std::vector<expression_parser> axioms;
    std::vector<expression_parser> assumptions;
    std::vector<expression_parser> proof;
    std::vector<std::string> end_proof;
    expression_parser final;

    global_parser() {
        add_axioms();
        parse_assump_and_final();
        parse_proof();
    }

    global_parser(std::string& a) {
        add_axioms();
        parse_add_assump_and_final(a);
        size_t tmp = assumptions.size();
    //    for (size_t i = 0; i < tmp; i++) {
      //      std::cout << assumptions[i].value << " " << std::endl << "   == " << std::endl << "          ";
      //      assumptions[i].root->draw();
    //    }
    }

    void add_axioms() {
        std::string s = "a->b->a";
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
        std::string s;
        getline(std::cin, s);
        s = my_split(s);
        std::string tmp = "";
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
        std::string s;
        while (getline(std::cin, s))
            if (s != "") {
                proof.push_back(expression_parser(s));
                std::pair<bool, std::string> t = check();
                if(!t.first) {
                    std::cout << "Вывод некоректен начиная с формулы номер " << proof.size() << t.second << std::endl;
                    return;
                }
            }
        if (proof[proof.size() - 1] == final) {
            size_t tmp = end_proof.size();
            for (size_t i = 0; i < tmp; i++)
                std::cout << end_proof[i] << std::endl;
        } else {
            std::cout << "Доказательство не окончено..." << std::endl;
        }
    }

    void parse_add_assump_and_final(std::string& s) {
        s = my_split(s);
        std::string tmp = "";
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

    std::vector<std::string> parse_add_proof(std::vector<std::string> t) {
        size_t tmp = t.size();
        std::string s;
        for (size_t i = 0; i < tmp; i++) {
            s = t[i];
            if (s != "") {
                proof.push_back(expression_parser(s));
                std::pair<bool, std::string> miy = check();
                if (!miy.first) {
                    break;
                }
            }
        }
        if (proof[proof.size() - 1] == final) {
            return end_proof;
        }
    }

    std::pair<bool, std::string> check() {
        if (is_assump())
            return std::make_pair(true, "");
        if (is_axiom())
            return std::make_pair(true, "");
        if (is_MP())
            return std::make_pair(true, "");
        std::pair<bool, std::string> miy = is_rule();
        if (miy.first || miy.second != "")
            return miy;
        miy = is_axiom_for_free();
        if (miy.first || miy.second != "")
            return miy;
        return std::make_pair(false, "");
    }

    bool is_assump() {
        size_t tmp = assumptions.size();
        expression_parser t = proof[proof.size() - 1];
        if (tmp > 1)
            for (size_t i = 0; i < tmp - 1; i++)
                if (t == assumptions[i]) {
                    end_proof.push_back(t.value);
                    end_proof.push_back("("+t.value+")->("+assumptions[tmp - 1].value+")->("+t.value+")");
                    end_proof.push_back("("+assumptions[tmp - 1].value+")->("+t.value+")");
                    return true;
                }
        if (tmp == 0)
            return false;
        else if (t == assumptions[tmp - 1]) {
            end_proof.push_back("("+t.value+")->("+t.value+")->("+t.value+")");
            end_proof.push_back("("+t.value+")->(("+t.value+")->("+t.value+"))->("+t.value+")");
            end_proof.push_back("(("+t.value+")->("+t.value+")->(" +t.value+"))->(("
                                +t.value+")->(("+t.value+")->("+t.value+"))->("+t.value+"))->(("
                                +t.value+")->("+t.value+"))");
            end_proof.push_back("(("+t.value+")->(("+t.value+")->("+t.value+"))->("+t.value+"))->(("
                                +t.value+")->("+t.value+"))");
            end_proof.push_back("("+t.value+")->("+t.value+")");
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
                end_proof.push_back("("+t.value+")->("+assumptions[assumptions.size() - 1].value+")->("+t.value+")");
                end_proof.push_back("("+assumptions[assumptions.size() - 1].value+")->("+t.value+")");
                return true;
            }
        for (size_t i = 10; i < 18; i++)
            if (*(t.root) == *(axioms[i].root)) {
                if (assumptions.size() == 0) {
                    end_proof.push_back(t.value);
                    return true;
                }
                end_proof.push_back(t.value);
                end_proof.push_back("("+t.value+")->("+assumptions[assumptions.size() - 1].value+")->("+t.value+")");
                end_proof.push_back("("+assumptions[assumptions.size() - 1].value+")->("+t.value+")");
                return true;
            }
        return false;
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

    std::pair<bool, std::string> is_rule() {
        expression_parser t = proof[proof.size() - 1];
        if (t.root->op == "->" && t.root->right->op == "@") {
            size_t tmp = proof.size();
            for (size_t i = 0; i < tmp; i++) {
                std::map<std::shared_ptr<expression>, std::shared_ptr<expression> > match;
                if (proof[i].root->op == "->" && *(proof[i].root->left) == *(t.root->left) &&
                    equal_axiom_(proof[i].root->right, t.root->right->right, match, t.root->right->left->value)) {
                    if (check_free(t.root->left, t.root->right->left->value))
                        return std::make_pair(false, ": используется правило с квантором по переменной " +
                                                     t.root->right->left->value + ", входящей свободно в допущение " +
                                                     t.root->right->right->value + ".");
                    if (assumptions.size() == 0)
                        end_proof.push_back(t.value);
                    else
                        add_to_end_proof(i);
                    return std::make_pair(true, "");
                }
            }
        } else if (t.root->op == "->" && t.root->left->op == "?") {
            size_t tmp = proof.size();
            for (size_t i = 0; i < tmp; i++) {
                std::map<std::shared_ptr<expression>, std::shared_ptr<expression> > match;
                if (proof[i].root->op == "->" && *(proof[i].root->right) == *(t.root->right) &&
                    equal_axiom_(proof[i].root->left, t.root->left->right, match, t.root->left->left->value)) {
                    if (check_free(t.root->right, t.root->left->left->value))
                        return std::make_pair(false, ": используется правило с квантором по переменной " +
                                                     t.root->left->left->value + ", входящей свободно в допущение " +
                                                     t.root->left->right->value + ".");
                    if (assumptions.size() == 0)
                        end_proof.push_back(t.value);
                    else
                        add_to_end_proof(i);
                    return std::make_pair(true, "");
                }
            }
        }
        return std::make_pair(false, "");
    }

    std::pair<bool, std::string> is_axiom_for_free() {
        expression_parser t = proof[proof.size() - 1];
        if (t.root->op == "->") {
            if (t.root->left->op == "@") {
                std::map< std::shared_ptr<expression>, std::shared_ptr<expression> > match;
                if (equal_axiom_(t.root->right, t.root->left->right, match, t.root->left->left->value)) {
                    std::map< std::shared_ptr<expression>, std::shared_ptr<expression> >::iterator it = match.begin();
                    for (;it != match.end(); it++)
                        if (*(it->first) == *(t.root->left->left))
                            break;
                    if (!check_free(t.root->left->right, it->second, t.root->left->left->value))
                        return std::make_pair(false, ": терм " + it->second->value + " не свободен для подстановки в формулу " + t.root->left->right->value + " вместо переменной " + t.root->left->left->value + ".");
                    if (assumptions.size() == 0) {
                        end_proof.push_back(t.value);
                        return std::make_pair(true, "");
                    }
                    end_proof.push_back(t.value);
                    end_proof.push_back("("+t.value+")->("+assumptions[assumptions.size() - 1].value+")->("+t.value+")");
                    end_proof.push_back("("+assumptions[assumptions.size() - 1].value+")->("+t.value+")");
                    return std::make_pair(true, "");
                }
            } else if (t.root->right->op == "?") {
                std::map< std::shared_ptr<expression>, std::shared_ptr<expression> > match;
                if (equal_axiom_(t.root->left, t.root->right->right, match, t.root->right->left->value)) {
                    std::map< std::shared_ptr<expression>, std::shared_ptr<expression> >::iterator it = match.begin();
                    for (;it != match.end(); it++)
                        if (*(it->first) == *(t.root->right->left))
                            break;
                    if (!check_free(t.root->right->right, it->second, t.root->right->left->value))
                        return std::make_pair(false, ": терм " + it->second->value + " не свободен для подстановки в формулу " + t.root->right->right->value + " вместо переменной " + t.root->right->left->value + ".");
                    if (assumptions.size() == 0) {
                        end_proof.push_back(t.value);
                        return std::make_pair(true, "");
                    }
                    end_proof.push_back(t.value);
                    end_proof.push_back("("+t.value+")->("+assumptions[assumptions.size() - 1].value+")->("+t.value+")");
                    end_proof.push_back("("+assumptions[assumptions.size() - 1].value+")->("+t.value+")");
                    return std::make_pair(true, "");
                }
            } else if (t.root->left->op == "&" && t.root->left->right->op == "@" && t.root->left->right->right->op == "->") {
                std::map< std::shared_ptr<expression>, std::shared_ptr<expression> > match;
                if (equal_axiom_(t.root->left->right->right->right,
                                 t.root->left->right->right->left,
                                 match, t.root->left->right->left->value)) {
                    std::map< std::shared_ptr<expression>, std::shared_ptr<expression> >::iterator it = match.begin();
                    for (it; it != match.end(); it++)
                        if (*(it->first) == *(t.root->left->right->left)) {
                            std::string st = "(" + t.root->left->right->left->value + ")\'";
                            expression_parser k1 = expression_parser(st);
                            expression k2 = *(it->second);
                            if (!(k2 == *(k1.root)))
                                return std::make_pair(false, "");
                        }
                    //*(match[t.root->left->right->left]) == *(expression_parser("("+t.root->left->right->left->value + ")\'").root)
                    std::map< std::shared_ptr<expression>, std::shared_ptr<expression> > match2, match3;
                    if (equal_axiom_(t.root->left->left, t.root->left->right->right->left, match2, t.root->left->right->left->value)) {
                        std::shared_ptr<expression> zer;
                        it = match2.begin();
                        for (it; it != match2.end(); it++)
                            if (*(it->first) == *(t.root->left->right->left)) {
                                zer = it->second;
                                break;
                            }

                        if (equal_axiom_(t.root->left->left, t.root->right, match3, zer->value)) {
                            if (!check_free(t.root->left->right->right->left, t.root->left->right->left->value))
                                return std::make_pair(false, ": переменная " + t.root->left->right->left->value +
                                                             " входит свободно в формулу " +
                                                             t.root->left->right->right->left->value + ".");
                            if (assumptions.size() == 0) {
                                end_proof.push_back(t.value);
                                return std::make_pair(true, "");
                            }
                            end_proof.push_back(t.value);
                            end_proof.push_back(
                                    "(" + t.value + ")->(" + assumptions[assumptions.size() - 1].value + ")->(" +
                                    t.value +
                                    ")");
                            end_proof.push_back(
                                    "(" + assumptions[assumptions.size() - 1].value + ")->(" + t.value + ")");
                            return std::make_pair(true, "");
                        }
                    }
                }
            }
        }
        return std::make_pair(false, "");
    }

    bool check_free(std::shared_ptr<expression> ex, std::string v) {
        return (ex->has_variable(v) && !ex->has_quantif_for_variable(v));
    }

    bool check_free(std::shared_ptr<expression> ex, std::shared_ptr<expression> ter, std::string v) {
        if ((ex->op == "@" || ex->op == "?") && ex->right->has_variable(v)) {
            std::vector<std::string> value_in_ter;
            ter->get_free_variable(value_in_ter);
            size_t n = value_in_ter.size();
            for (size_t i = 0; i < n; i++)
                if (ex->left->value == value_in_ter[i] && !ex->right->has_variable(value_in_ter[i]))
                    return false;
        }
        if (ex->left != NULL && !check_free(ex->left, ter, v))
            return false;
        if (ex->right != NULL && !check_free(ex->right, ter, v))
            return false;
    //    return ex->has_variable(v);
        return true;
    }


    void add_to_end_proof(size_t id) {
        expression_parser t = proof[proof.size() - 1], a = assumptions[assumptions.size() - 1];
        if (t.root->right->op == "@") {
            std::string ass;
            std::shared_ptr<expression> b = proof[id].root->left, g = proof[id].root->right;
            ass = "(" + a.value + ")->(" + proof[id].root->value + "),(" + a.value +")&(" + b->value + ")|-" + g->value;
            global_parser tmp(ass);
            std::vector<std::string> pr;
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
            std::vector<std::string> pr2;
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
            std::string ass;
            std::shared_ptr<expression> b = proof[id].root->left, g = proof[id].root->right;
            ass = "(" + a.value + ")->(" + b->value + ")->(" + g->value + ")," + b->value + "," + a.value + "|-" + g->value;
            global_parser tmp(ass);
            std::vector<std::string> pr;
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
            std::vector<std::string> pr2;
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
    bool equal_axiom(expression_parser ex, expression_parser ax) {
        std::map< std::shared_ptr<expression>, std::shared_ptr<expression> > match;
        return equal_axiom(ex.root, ax.root, match);
    }
    bool equal_axiom(std::shared_ptr<expression> ex, std::shared_ptr<expression> ax, std::map< std::shared_ptr<expression>, std::shared_ptr<expression> >& match) {
        if (ax->left == NULL && ax->right == NULL) {
            std::map< std::shared_ptr<expression>, std::shared_ptr<expression> >::iterator it = match.begin();
            for (it; it != match.end(); it++)
                if (*(it->first) == *ax) {
                return (*(it->second) == *ex);
            }
            match.insert(std::make_pair(ax, ex));
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
    bool equal_axiom_(std::shared_ptr<expression> ex, std::shared_ptr<expression> ax, std::map< std::shared_ptr<expression>, std::shared_ptr<expression> >& match, std::string v) {
        if (ax->left == NULL && ax->right == NULL) {
            if (ax->value == v || ax->value == ex->value) {
                std::map<std::shared_ptr<expression>, std::shared_ptr<expression> >::iterator it = match.find(ax);
                if (it == match.end()) {
                    match.insert(std::make_pair(ax, ex));
                    return true;
                } else {
                    return (*match[ax] == *ex);
                }
            }
            return false;
        } else {
            if (ex->op == ax->op)
                if (ex->op != "") {
                    if (ex->left == NULL || equal_axiom(ex->left, ax->left, match))
                        if (ex->right == NULL || equal_axiom(ex->right, ax->right, match))
                                return true;
                } else
                    if (ax->left->value == ex->left->value && ax->terms.size() == ex->terms.size()) {
                        size_t  tmp = ex->terms.size();
                        for (size_t i = 0; i < tmp; i++)
                            if (!equal_axiom_(ex->terms[i], ax->terms[i], match, v))
                                return false;
                        return true;
                    }
        }
        return false;
    }

private:
    std::string my_split(std::string t) {
        std::string tmp;
        size_t n = t.size();
        for (size_t i = 0; i < n; i++)
            if (t[i] != ' ')
                tmp.push_back(t[i]);
        return tmp;
    }

};

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    global_parser tmp;
    return 0;
}