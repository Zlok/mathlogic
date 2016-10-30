#include <iostream>
#include <string>
#include <set>
#include <map>

using namespace std;

string head;
set<string> assump;
string state;
map<string, unsigned> assump_number;

string my_split(string tmp) {
    string t;
    size_t i = 0, n = tmp.size();
    while (i < n) {
        if (tmp[i] != ' ')
            t.push_back(tmp[i]);
        i++;
    }
    return t;
}

string brackets_check(string tmp) {
    string t;
    int count = 0;
    if (tmp[0] == '(' && tmp[tmp.size() - 1] == ')') {
        size_t i = 1, n = tmp.size() - 1;
        while (i < n) {
            t.push_back(tmp[i]);
            if (tmp[i] == '(')
                count++;
            else if (tmp[i] == ')') {
                if (count == 0)
                    return tmp;
                count--;
            }
            i++;
        }
        return t;
    }
    return tmp;
}

void parse_head() {
    size_t n = head.size();
    size_t i = 0;
    unsigned count = 0;
    string tmp;
    while (i < n) {
        if (head[i] == '|' && head[i + 1] == '-') {
            if (tmp.size()) {
                tmp = brackets_check(tmp);
                assump.insert(tmp);
                count++;
                assump_number.insert(make_pair(tmp, count));
                tmp.clear();
                tmp.resize(0);
            }
            i += 2;
        } else if (head[i] == ',') {
            tmp = brackets_check(tmp);
            assump.insert(tmp);
            count++;
            assump_number.insert(make_pair(tmp, count));
            tmp.clear();
            tmp.resize(0);
            i++;
        } else if (head[i] != ' ') {
            tmp.push_back(head[i]);
            i++;
        } else {
            i++;
        }
    }
    tmp = brackets_check(tmp);
    state = tmp;
}

string s;
set<string> proof;
map<string, unsigned> proof_number;

pair<string, string> parse_two(string tmp) {
    size_t n = tmp.size(), i = 0;
    string left, right;
    unsigned count = 0;
    bool flag = false;
    while (i < n) {
        if (!flag) {
            if (tmp[i] == '(') {
                count++;
                left.push_back(tmp[i]);
                i++;
            } else if (tmp[i] == ')') {
                count--;
                left.push_back(tmp[i]);
                i++;
            } else if (count == 0 && tmp[i] == '-' && tmp[i + 1] == '>') {
                flag = true;
                i += 2;
            } else {
                left.push_back(tmp[i]);
                i++;
            }
        } else {
            right.push_back(tmp[i]);
            i++;
        }
    }
    left = brackets_check(left);
    right = brackets_check(right);
    return make_pair(left, right);
}

pair<string, pair<string, string> > parse_three(string tmp) {
    string t1, t2, t3;
    int flag = 0;
    int count = 0;
    size_t n = tmp.size(), i = 0;
    while (i < n) {
        if (!flag) {
            if (tmp[i] == '(') {
                count++;
                t1.push_back(tmp[i]);
                i++;
            } else if (tmp[i] == ')') {
                count--;
                t1.push_back(tmp[i]);
                i++;
            } else if (count == 0 && tmp[i] == '-' && tmp[i + 1] == '>') {
                flag = 1;
                i += 2;
            } else {
                t1.push_back(tmp[i]);
                i++;
            }
        } else if (flag == 1) {
            if (tmp[i] == '(') {
                count++;
                t2.push_back(tmp[i]);
                i++;
            } else if (tmp[i] == ')') {
                count--;
                t2.push_back(tmp[i]);
                i++;
            } else if (count == 0 && tmp[i] == '-' && tmp[i + 1] == '>') {
                flag = 2;
                i += 2;
            } else {
                t2.push_back(tmp[i]);
                i++;
            }
        } else {
            t3.push_back(tmp[i]);
            i++;
        }
    }
    t1 = brackets_check(t1);
    t2 = brackets_check(t2);
    t3 = brackets_check(t3);
    return make_pair(t1, make_pair(t2, t3));
}

string parse_not(string tmp) {
    if (tmp[0] == '!') {
        string t;
        size_t n = tmp.size();
        for (size_t i = 1; i < n; i++)
            t.push_back(tmp[i]);
        t = brackets_check(t);
        return t;
    }
}

bool is_assumption(string st) {
    set<string>::iterator it = assump.find(st);
    return it != assump.end();
}

bool is_1_axiom(string tmp) {
    pair<string, pair<string, string> > t = parse_three(tmp);
    if (t.second.second.size()) {
        t.first = brackets_check(t.first);
        t.second.second = brackets_check(t.second.second);
        if (t.first == t.second.second)
            return true;
    }
    return false;
}

bool is_2_axiom(string tmp) {
    pair<string, pair<string, string> > t = parse_three(tmp);
    if (t.second.second.size()) {
        t.first = brackets_check(t.first);
        t.second.first = brackets_check(t.second.first);
        t.second.second = brackets_check(t.second.second);
        pair<string, string> t1 = parse_two(t.first);
        pair<string, pair<string, string> > t2 = parse_three(t.second.first);
        pair<string, string> t3 = parse_two(t.second.second);
        if (t1.second.size() && t2.second.second.size() && t3.second.size()) {
            if (t1.first == t2.first && t1.second == t2.second.first && t1.first == t3.first && t2.second.second == t3.second)
                return true;
        }
    }
    return false;
}

bool is_3_axiom(string tmp) {
    pair<string, pair<string, string> > t = parse_three(tmp);
    if (t.second.second.size()) {
        if (t.second.second.size() == t.first.size() + t.second.first.size() + 1) {
            size_t n1 = t.first.size(), n2 = t.second.first.size();
            for (size_t i = 0; i < n1; i++)
                if (t.first[i] != t.second.second[i])
                    return false;
            for (size_t i = 0; i < n2; i++)
                if (t.second.first[i] != t.second.second[i + n1 + 1])
                    return false;
            if (t.second.second[n1] == '&')
                return true;
        } else if (t.second.second.size() == t.first.size() + t.second.first.size() + 5 &&
                t.second.second[0] == '(' && t.second.second[t.first.size() + 1] == ')' &&
                t.second.second[t.first.size() + 3] == '(' && t.second.second[t.second.second.size() - 1] == ')') {
            size_t n1 = t.first.size(), n2 = t.second.first.size();
            for (size_t i = 0; i < n1; i++)
                if (t.first[i] != t.second.second[i + 1])
                    return false;
            for (size_t i = 0; i < n2; i++)
                if (t.second.first[i] != t.second.second[i + n1 + 4])
                    return false;
            if (t.second.second[n1 + 2] == '&')
                return true;
        } else if (t.second.second.size() == t.first.size() + t.second.first.size() + 3 &&
                   t.second.second[0] == '(' && t.second.second[t.first.size() + 1] == ')') {
            size_t n1 = t.first.size(), n2 = t.second.first.size();
            for (size_t i = 0; i < n1; i++)
                if (t.first[i] != t.second.second[i + 1])
                    return false;
            for (size_t i = 0; i < n2; i++)
                if (t.second.first[i] != t.second.second[i + n1 + 3])
                    return false;
            if (t.second.second[n1 + 2] == '&')
                return true;
        } else if (t.second.second.size() == t.first.size() + t.second.first.size() + 5 &&
                   t.second.second[t.first.size() + 3] == '(' && t.second.second[t.second.second.size() - 1] == ')') {
            size_t n1 = t.first.size(), n2 = t.second.first.size();
            for (size_t i = 0; i < n1; i++)
                if (t.first[i] != t.second.second[i])
                    return false;
            for (size_t i = 0; i < n2; i++)
                if (t.second.first[i] != t.second.second[i + n1 + 2])
                    return false;
            if (t.second.second[n1] == '&')
                return true;
        }
    }
    return false;
}

bool is_4_axiom(string tmp) {
    pair<string, string> t = parse_two(tmp);
    if (t.second.size()) {
        if (t.first.size() > t.second.size()) {
            size_t n = t.second.size();
            if (t.first[n] == '&') {
                for (size_t i = 0; i < n; i++)
                    if (t.first[i] != t.second[i])
                        return false;
                return true;
            } else if (t.first[n + 2] == '&' && t.first[0] == '(' && t.first[n + 1] == ')') {
                for (size_t i = 0; i < n; i++)
                    if (t.first[i + 1] != t.second[i])
                        return false;
                return true;
            }
        }
    }
    return false;
}

bool is_5_axiom(string tmp) {
    pair<string, string> t = parse_two(tmp);
    if (t.second.size()) {
        if (t.first.size() > t.second.size()) {
            size_t n2 = t.second.size(), n1 = t.first.size();
            if (t.first[n1 - n2 - 1] == '&') {
                for (size_t i = 1; i <= n2; i++)
                    if (t.first[n1 - i] != t.second[n2 - i])
                        return false;
                return true;
            } else if (t.first[n1 - n2 - 3] == '&' && t.first[n1 - 1] == ')' && t.first[n1 - n2 - 2] == '(') {
                for (size_t i = 1; i <= n2; i++)
                    if (t.first[n1 - i - 1] != t.second[n2 - i])
                        return false;
                return true;
            }
        }
    }
    return false;
}

bool is_6_axiom(string tmp) {
    pair<string, string> t = parse_two(tmp);
    if (t.second.size()) {
        if (t.first.size() < t.second.size()) {
            size_t n = t.first.size();
            if (t.second[n] == '|') {
                for (size_t i = 0; i < n; i++)
                    if (t.first[i] != t.second[i])
                        return false;
                return true;
            } else if (t.second[0] == '(' && t.second[n + 1] == ')' && t.second[n + 2] == '|') {
                for (size_t i = 0; i < n; i++)
                    if (t.first[i] != t.second[i + 1])
                        return false;
                return true;
            }
        }
    }
    return false;
}

bool is_7_axiom(string tmp) {
    pair<string, string> t = parse_two(tmp);
    if (t.second.size()) {
        if (t.first.size() < t.second.size()) {
            size_t n1 = t.first.size(), n2 = t.second.size();
            if (t.second[n2 - n1 - 1] == '|') {
                for (size_t i = 1; i <= n1; i++)
                    if (t.first[n1 - i] != t.second[n2 - i])
                        return false;
                return true;
            } else if (t.second[n2 - 1] == ')' && t.second[n2 - n1 - 2] == '(' && t.second[n2 - n1 -3] == '|') {
                for (size_t i = 1; i <= n1; i++)
                    if (t.first[n1 - i] != t.second[n2 - i - 1])
                        return false;
                return true;
            }
        }
    }
    return false;
}

bool is_8_axiom(string tmp) {
    pair<string, pair<string, string> > t = parse_three(tmp);
    if (t.second.second.size()) {
        pair<string, string> t1 = parse_two(t.first), t2 = parse_two(t.second.first), t3 = parse_two(t.second.second);
        if (!t1.second.size() || !t2.second.size() || !t3.second.size())
            return false;
        if (t1.second == t2.second && t2.second == t3.second) {
            size_t n1 = t1.first.size(), n2 = t2.first.size(), n3 = t3.first.size();
            if (n3 <= n1 + n2)
                return false;
            if (n3 == n1 + n2 + 1) {
                for (size_t i = 0; i < n1; i++)
                    if (t1.first[i] != t3.first[i])
                        return false;
                for (size_t i = 0; i < n2; i++)
                    if (t2.first[i] != t3.first[n1 + i + 1])
                        return false;
                return t3.first[n1] == '|';
            } else if (n3 == n1 + n2 + 5 && t3.first[0] == '(' && t3.first[n1 + 1] == ')' && t3.first[n1 + 3] == '(' && t3.first[t3.first.size() - 1] == ')') {
                for (size_t i = 0; i < n1; i++)
                    if (t1.first[i] != t3.first[i + 1])
                        return false;
                for (size_t i = 0; i < n2; i++)
                    if (t2.first[i] != t3.first[n1 + i + 4])
                        return false;
                return t3.first[n1 + 2] == '|';
            } else if (n3 == n1 + n2 + 3 && t3.first[0] == '(' && t3.first[n1 + 1] == ')') {
                for (size_t i = 0; i < n1; i++)
                    if (t1.first[i] != t3.first[i + 1])
                        return false;
                for (size_t i = 0; i < n2; i++)
                    if (t2.first[i] != t3.first[n1 + i + 3])
                        return false;
                return t3.first[n1 + 2] == '|';
            } else if (n3 == n1 + n2 + 3 && t3.first[n1 + 1] == '(' && t3.first[t3.first.size() - 1] == ')') {
                for (size_t i = 0; i < n1; i++)
                    if (t1.first[i] != t3.first[i])
                        return false;
                for (size_t i = 0; i < n2; i++)
                    if (t2.first[i] != t3.first[n1 + i + 1])
                        return false;
                return t3.first[n1] == '|';
            }
        }
    }
    return false;
}

bool is_9_axiom(string tmp) {
    pair<string, pair<string,string> > t = parse_three(tmp);
    if (t.second.second.size()) {
        pair<string,string> t1 = parse_two(t.first), t2 = parse_two(t.second.first);
        if (t1.second.size() && t2.second.size()) {
            if (t1.first == t2.first && t1.second == parse_not(t2.second) && t1.first == parse_not(t.second.second))
                return true;
        }
    }
    return false;
}

bool is_10_axiom(string tmp) {
    pair<string, string> t = parse_two(tmp);
    if (t.first[0] != '!')
        return false;
    string s1 = parse_not(t.first);
    if (s1[0] != '!')
        return false;
    if (parse_not(s1) == t.second)
        return true;
    return false;
}

int is_axiom() {
    if (is_1_axiom(s))
        return 1;
    if (is_2_axiom(s))
        return 2;
    if (is_3_axiom(s))
        return 3;
    if (is_4_axiom(s))
        return 4;
    if (is_5_axiom(s))
        return 5;
    if (is_6_axiom(s))
        return 6;
    if (is_7_axiom(s))
        return 7;
    if (is_8_axiom(s))
        return 8;
    if (is_9_axiom(s))
        return 9;
    if (is_10_axiom(s))
        return 10;
    return 0;
}

pair <int, int> is_mp() {
    set<string>::iterator it = proof.begin();
    for (; it != proof.end(); it++) {
        pair<string, string> t = parse_two(*it);
        if (t.second.size()) {
            t.first = brackets_check(t.first);
            t.second = brackets_check(t.second);
            if (t.second == s) {
                set<string>::iterator my = proof.find(t.first);
                if (my != proof.end())
                    return make_pair(proof_number[*it], proof_number[t.first]);
            }
        }
    }
    return make_pair(0, 0);
}

void check() {
    static int counter = 1;
    cout << "(" << counter << ") ";
    if (is_assumption(s)) {
        cout << s << " " << "(Предп. " << assump_number[s] << ")" << endl;
    } else {
        int tmp_a = is_axiom();
        if (tmp_a) {
            cout << s << " " << "(Сх. акс. " << tmp_a << ")" << endl;
        } else {
            pair<int, int> tmp_mp = is_mp();
            if (tmp_mp.first) {
                cout << s << " " << "(M.P " << tmp_mp.first << ", " << tmp_mp.second << ")" << endl;
            } else {
                cout << s << " " << "(Не доказано)" << endl;
            }
        }
    }
    proof.insert(s);
    proof_number.insert(make_pair(s, counter));
    counter++;
}

int main() {
//    freopen("input.txt", "r", stdin);
//    freopen("output.txt", "w", stdout);
    getline(cin, head);
    head = my_split(head);
    parse_head();
/*    set<string>::iterator it = assump.begin();
    for (; it != assump.end(); it++) {
        cout << *it  << " " << assump_number[*it] << endl;
    }
    cout << state << endl;
*/
    while (getline(cin, s)) {
        s = my_split(s);
        check();
    }
    return 0;
}