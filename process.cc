#include "process.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <stdio.h>
#include <stdlib.h>

namespace proc{
    std::vector<std::string> method_name;
    std::map<std::string, _t_method> method;
    std::map<std::string, std::string> config;
    std::map<std::string, std::string> data;
    std::map<std::string, std::string> db0, db1;
}
std::string find (const proc::_t_method &, const guint &);
std::string get (const std::string &, int loc);
std::string set (const std::string &, int, const std::string &);
std::string ret_char_at (GtkTextBuffer *, const GtkTextIter &);
gboolean dec(GtkTextIter&);
void set_char_at (GtkTextBuffer *, GtkTextIter &, const std::string &);
GtkTextIter ret_insert_iter (GtkTextBuffer *);
std::string ascii_to_up_case(const std::string &);
std::string ascii_to_low_case(const std::string &);

int read_config ();
int read_db ();
///////////////////////////////////////////////////////////////////////////
std::string ascii_to_up_case(const std::string &s) {
    using namespace std;
    string ss = s;
    for (int i=0;i<ss.length(); i++) if (ss[i]>='a' && ss[i]<='z') ss[i]=ss[i]-0x20;
    return ss;
}
std::string ascii_to_low_case(const std::string &s) {
    using namespace std;
    string ss = s;
    for (int i=0;i<ss.length(); i++) if (ss[i]>='A' && ss[i]<='Z') ss[i]=ss[i]+0x20;
    return ss;
}
void set_char_at (GtkTextBuffer *buffer, GtkTextIter &i, const std::string &s) {
    gtk_text_buffer_backspace (buffer, &i, FALSE, TRUE);
    gtk_text_buffer_insert (buffer, &i, &(s[0]), s.length());
}
GtkTextIter ret_insert_iter (GtkTextBuffer *buffer) {
    GtkTextIter i;
    gtk_text_buffer_get_iter_at_mark (
        GTK_TEXT_BUFFER (buffer),
        &i,
        gtk_text_buffer_get_mark (buffer, "insert")
    );
    return i;
}
gboolean dec(GtkTextIter &i){
    return gtk_text_iter_backward_char (&i);
}
std::string ret_char_at (GtkTextBuffer *buffer, const GtkTextIter &i) {
    GtkTextIter j = i;
    dec(j);
    char *c = gtk_text_buffer_get_text (buffer, &j, &i, false);
    std::string s(c);
    delete[] c;
    return s;
}
std::string set (const std::string & s, int loc, const std::string &value) {
    using namespace std;
    string g = get (s, loc);
    if (g != "") return regex_replace (s, regex("[("+g+"]"), value);
    if (loc == 2 || s.length() == 1) return s+value;
    if (loc == 1) return string(s).insert(0, value);
}
std::string get (const std::string &s, int loc) {
    using namespace std;
    if (loc == 0) {
        return s.substr(0, 1);
    }else if (loc == 1 && regex_search(s, regex(R"([(*^-])"))) {
        return regex_replace (s, regex("([(*^-])"), "$1", regex_constants::format_no_copy);
    }else if (loc == 2 && regex_search(s, regex("[`'?~.]"))) {
        return regex_replace (s, regex("([`'?~.])"), "$1", regex_constants::format_no_copy);
    }
    return string("");
}
std::string find (const std::map<std::string, guint>& m, const guint& s) {
    for (auto i = m.begin(); i!=m.end(); i++){
        if (i->second == s) return i->first;
    }
    return "NULL";
}

int proc::init () {
    read_config ();
    read_db ();
}

int read_config (){
    using namespace std;
    using namespace proc;
    ifstream in ("/etc/vntyper.cfg");
    string s;
    bool reading_method = false;
    while (!in.eof()){
        getline(in, s);
        if (s.length() == 0) continue;

        if (reading_method && regex_match(s, regex(R"(^[^ ]{1,2}\s*[^ ]\s*$)"))) {
            string l = regex_replace (s, regex(R"(^([^ ]*))"), "$1", regex_constants::format_no_copy);
            string r = regex_replace (s, regex( R"(([^ ])\s*$)" ), "$1", regex_constants::format_no_copy);
            method[method_name.back()][l] = r[0];
            if (l.length()==2) method[method_name.back()][ascii_to_up_case(l)] = r[0];
            continue;
        }

        s = regex_replace (s, regex(R"(\s*)"), "");
        if (s[0] == '[' && s[s.length()-1] == ']' && s.length()>2){
            reading_method = !reading_method;
            if (reading_method) method_name.push_back (s.substr(1, s.length()-2));
            continue;
        }

        if (!reading_method && regex_match(s, regex(R"(^\w*=\w*$)"))) {
            string l = regex_replace (s, regex(R"(^(\w*))"), "$1", regex_constants::format_no_copy);
            string r = regex_replace (s, regex(R"((\w*)$)"), "$1", regex_constants::format_no_copy);
            config[l] = r;
            continue;
        }
    }
    if (config.count("DEFAULT") == 0) config["DEFAULT"] = method_name[0];
    if (config.count("WIDTH") == 0) config["WIDTH"] = "300";
    if (config.count("HEIGHT") == 0) config["HEIGHT"] = "400";
    config["CURRENT"] = config["DEFAULT"];
    return 0;
}

int read_db () {

    using namespace proc;
    db1 = {
        {"a'", "á"}, {"a`", "à"}, {"a?", "ả"}, {"a~", "ã"}, 
        {"a.", "ạ"}, {"a(", "ă"}, {"a('", "ắ"}, {"a(`", "ằ"}, 
        {"a(?", "ẳ"}, {"a(~", "ẵ"}, {"a(.", "ặ"}, {"a^", "â"}, 
        {"a^'", "ấ"}, {"a^`", "ầ"}, {"a^?", "ẩ"}, {"a^~", "ẫ"}, 
        {"a^.", "ậ"}, {"e'", "é"}, {"e`", "è"}, {"e?", "ẻ"}, 
        {"e~", "ẽ"}, {"e.", "ẹ"}, {"e^", "ê"}, {"e^'", "ế"}, 
        {"e^`", "ề"}, {"e^?", "ể"}, {"e^~", "ễ"}, {"e^.", "ệ"}, 
        {"o'", "ó"}, {"o`", "ò"}, {"o?", "ỏ"}, {"o~", "õ"}, 
        {"o.", "ọ"}, {"o^", "ô"}, {"o^'", "ố"}, {"o^`", "ồ"}, 
        {"o^?", "ổ"}, {"o^~", "ỗ"}, {"o^.", "ộ"}, {"o*", "ơ"}, 
        {"o*'", "ớ"}, {"o*`", "ờ"}, {"o*?", "ở"}, {"o*~", "ỡ"}, 
        {"o*.", "ợ"}, {"u'", "ú"}, {"u`", "ù"}, {"u?", "ủ"}, 
        {"u~", "ũ"}, {"u.", "ụ"}, {"u*", "ư"}, {"u*'", "ứ"}, 
        {"u*`", "ừ"}, {"u*?", "ử"}, {"u*~", "ữ"}, {"u*.", "ự"}, 
        {"y'", "ý"}, {"y`", "ỳ"}, {"y?", "ỷ"}, {"y~", "ỹ"}, 
        {"y.", "ỵ"}, {"i'", "í"}, {"i`", "ì"}, {"i?", "ỉ"}, 
        {"i~", "ĩ"}, {"i.", "ị"}, {"d-", "đ"}, {"a", "a"}, 
        {"b", "b"}, {"c", "c"}, {"d", "d"}, {"e", "e"}, 
        {"f", "f"}, {"g", "g"}, {"h", "h"}, {"i", "i"}, 
        {"j", "j"}, {"k", "k"}, {"l", "l"}, {"m", "m"}, 
        {"n", "n"}, {"o", "o"}, {"p", "p"}, {"q", "q"}, 
        {"r", "r"}, {"s", "s"}, {"t", "t"}, {"u", "u"}, 
        {"v", "v"}, {"w", "w"}, {"x", "x"}, {"y", "y"}, 
        {"z", "z"}, {"A'", "Á"}, {"A`", "À"}, {"A?", "Ả"}, 
        {"A~", "Ã"}, {"A.", "Ạ"}, {"A(", "Ă"}, {"A('", "Ắ"}, 
        {"A(`", "Ằ"}, {"A(?", "Ẳ"}, {"A(~", "Ẵ"}, {"A(.", "Ặ"}, 
        {"A^", "Â"}, {"A^'", "Ấ"}, {"A^`", "Ầ"}, {"A^?", "Ẩ"}, 
        {"A^~", "Ẫ"}, {"A^.", "Ậ"}, {"E'", "É"}, {"E`", "È"}, 
        {"E?", "Ẻ"}, {"E~", "Ẽ"}, {"E.", "Ẹ"}, {"E^", "Ê"}, 
        {"E^'", "Ế"}, {"E^`", "Ề"}, {"E^?", "Ể"}, {"E^~", "Ễ"}, 
        {"E^.", "Ệ"}, {"O'", "Ó"}, {"O`", "Ò"}, {"O?", "Ỏ"}, 
        {"O~", "Õ"}, {"O.", "Ọ"}, {"O^", "Ô"}, {"O^'", "Ố"}, 
        {"O^`", "Ồ"}, {"O^?", "Ổ"}, {"O^~", "Ỗ"}, {"O^.", "Ộ"}, 
        {"O*", "Ơ"}, {"O*'", "Ớ"}, {"O*`", "Ờ"}, {"O*?", "Ở"}, 
        {"O*~", "Ỡ"}, {"O*.", "Ợ"}, {"U'", "Ú"}, {"U`", "Ù"}, 
        {"U?", "Ủ"}, {"U~", "Ũ"}, {"U.", "Ụ"}, {"U*", "Ư"}, 
        {"U*'", "Ứ"}, {"U*`", "Ừ"}, {"U*?", "Ử"}, {"U*~", "Ữ"}, 
        {"U*.", "Ự"}, {"Y'", "Ý"}, {"Y`", "Ỳ"}, {"Y?", "Ỷ"}, 
        {"Y~", "Ỹ"}, {"Y.", "Ỵ"}, {"I'", "Í"}, {"I`", "Ì"}, 
        {"I?", "Ỉ"}, {"I~", "Ĩ"}, {"I.", "Ị"}, {"D-", "Đ"}, 
        {"A", "A"}, {"B", "B"}, {"C", "C"}, {"D", "D"}, 
        {"E", "E"}, {"F", "F"}, {"G", "G"}, {"H", "H"}, 
        {"I", "I"}, {"J", "J"}, {"K", "K"}, {"L", "L"}, 
        {"M", "M"}, {"N", "N"}, {"O", "O"}, {"P", "P"}, 
        {"Q", "Q"}, {"R", "R"}, {"S", "S"}, {"T", "T"}, 
        {"U", "U"}, {"V", "V"}, {"W", "W"}, {"X", "X"}, 
        {"Y", "Y"}, {"Z", "Z"}
    };
    db0 = {
        {"á", "a'"}, {"à", "a`"}, {"ả", "a?"}, {"ã", "a~"}, 
        {"ạ", "a."}, {"ă", "a("}, {"ắ", "a('"}, {"ằ", "a(`"}, 
        {"ẳ", "a(?"}, {"ẵ", "a(~"}, {"ặ", "a(."}, {"â", "a^"}, 
        {"ấ", "a^'"}, {"ầ", "a^`"}, {"ẩ", "a^?"}, {"ẫ", "a^~"}, 
        {"ậ", "a^."}, {"é", "e'"}, {"è", "e`"}, {"ẻ", "e?"}, 
        {"ẽ", "e~"}, {"ẹ", "e."}, {"ê", "e^"}, {"ế", "e^'"}, 
        {"ề", "e^`"}, {"ể", "e^?"}, {"ễ", "e^~"}, {"ệ", "e^."}, 
        {"ó", "o'"}, {"ò", "o`"}, {"ỏ", "o?"}, {"õ", "o~"}, 
        {"ọ", "o."}, {"ô", "o^"}, {"ố", "o^'"}, {"ồ", "o^`"}, 
        {"ổ", "o^?"}, {"ỗ", "o^~"}, {"ộ", "o^."}, {"ơ", "o*"}, 
        {"ớ", "o*'"}, {"ờ", "o*`"}, {"ở", "o*?"}, {"ỡ", "o*~"}, 
        {"ợ", "o*."}, {"ú", "u'"}, {"ù", "u`"}, {"ủ", "u?"}, 
        {"ũ", "u~"}, {"ụ", "u."}, {"ư", "u*"}, {"ứ", "u*'"}, 
        {"ừ", "u*`"}, {"ử", "u*?"}, {"ữ", "u*~"}, {"ự", "u*."}, 
        {"ý", "y'"}, {"ỳ", "y`"}, {"ỷ", "y?"}, {"ỹ", "y~"}, 
        {"ỵ", "y."}, {"í", "i'"}, {"ì", "i`"}, {"ỉ", "i?"}, 
        {"ĩ", "i~"}, {"ị", "i."}, {"đ", "d-"}, {"a", "a"}, 
        {"b", "b"}, {"c", "c"}, {"d", "d"}, {"e", "e"}, 
        {"f", "f"}, {"g", "g"}, {"h", "h"}, {"i", "i"}, 
        {"j", "j"}, {"k", "k"}, {"l", "l"}, {"m", "m"}, 
        {"n", "n"}, {"o", "o"}, {"p", "p"}, {"q", "q"}, 
        {"r", "r"}, {"s", "s"}, {"t", "t"}, {"u", "u"}, 
        {"v", "v"}, {"w", "w"}, {"x", "x"}, {"y", "y"}, 
        {"z", "z"}, {"Á", "A'"}, {"À", "A`"}, {"Ả", "A?"}, 
        {"Ã", "A~"}, {"Ạ", "A."}, {"Ă", "A("}, {"Ắ", "A('"}, 
        {"Ằ", "A(`"}, {"Ẳ", "A(?"}, {"Ẵ", "A(~"}, {"Ặ", "A(."}, 
        {"Â", "A^"}, {"Ấ", "A^'"}, {"Ầ", "A^`"}, {"Ẩ", "A^?"}, 
        {"Ẫ", "A^~"}, {"Ậ", "A^."}, {"É", "E'"}, {"È", "E`"}, 
        {"Ẻ", "E?"}, {"Ẽ", "E~"}, {"Ẹ", "E."}, {"Ê", "E^"}, 
        {"Ế", "E^'"}, {"Ề", "E^`"}, {"Ể", "E^?"}, {"Ễ", "E^~"}, 
        {"Ệ", "E^."}, {"Ó", "O'"}, {"Ò", "O`"}, {"Ỏ", "O?"}, 
        {"Õ", "O~"}, {"Ọ", "O."}, {"Ô", "O^"}, {"Ố", "O^'"}, 
        {"Ồ", "O^`"}, {"Ổ", "O^?"}, {"Ỗ", "O^~"}, {"Ộ", "O^."}, 
        {"Ơ", "O*"}, {"Ớ", "O*'"}, {"Ờ", "O*`"}, {"Ở", "O*?"}, 
        {"Ỡ", "O*~"}, {"Ợ", "O*."}, {"Ú", "U'"}, {"Ù", "U`"}, 
        {"Ủ", "U?"}, {"Ũ", "U~"}, {"Ụ", "U."}, {"Ư", "U*"}, 
        {"Ứ", "U*'"}, {"Ừ", "U*`"}, {"Ử", "U*?"}, {"Ữ", "U*~"}, 
        {"Ự", "U*."}, {"Ý", "Y'"}, {"Ỳ", "Y`"}, {"Ỷ", "Y?"}, 
        {"Ỹ", "Y~"}, {"Ỵ", "Y."}, {"Í", "I'"}, {"Ì", "I`"}, 
        {"Ỉ", "I?"}, {"Ĩ", "I~"}, {"Ị", "I."}, {"Đ", "D-"}, 
        {"A", "A"}, {"B", "B"}, {"C", "C"}, {"D", "D"}, 
        {"E", "E"}, {"F", "F"}, {"G", "G"}, {"H", "H"}, 
        {"I", "I"}, {"J", "J"}, {"K", "K"}, {"L", "L"}, 
        {"M", "M"}, {"N", "N"}, {"O", "O"}, {"P", "P"}, 
        {"Q", "Q"}, {"R", "R"}, {"S", "S"}, {"T", "T"}, 
        {"U", "U"}, {"V", "V"}, {"W", "W"}, {"X", "X"}, 
        {"Y", "Y"}, {"Z", "Z"}, 
    };
    return 0;
}

void proc::destroy (){
    gtk_main_quit ();
}
int proc::print_and_close (GtkTextBuffer *buffer){
    GtkTextIter start;
    GtkTextIter end;
    gchar *text;
    gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (buffer), &start);
    gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (buffer), &end);
    text = gtk_text_buffer_get_text (GTK_TEXT_BUFFER (buffer), &start, &end, FALSE);
    printf ("%s", text);
    g_free (text);
    destroy ();
    return 0;
}

gboolean proc::keypress (GtkWidget *text_view, GdkEventKey *event, GtkTextBuffer *buffer){
    using namespace std;
    auto key = event->keyval;
    if ((key == 's' || key == 'd' || key == 'S' || key == 'D') && (event->state & GDK_CONTROL_MASK)){
        print_and_close(buffer);
        return TRUE;
    }
    if (key == 'q' && (event->state & GDK_CONTROL_MASK)){
        destroy();
        return TRUE;
    }
    if (event->state & GDK_CONTROL_MASK) return FALSE;
    if (key>='A' && key<='Z') key+=0x20;
    if (find(method[config["CURRENT"]], key) == "NULL") return FALSE;

    if (find(method[config["CURRENT"]], key).length() == 1) {
        auto dat_dau = [&, buffer, key](GtkTextIter i)->gboolean{
            gboolean ret;
            string c = db0[ret_char_at (buffer, i)];
            string dau = find(method[config["CURRENT"]], key);
            if (get(c, 2) == dau) {
                c = set(c, 2, "");
                ret = FALSE;
            }else{
                c = set(c, 2, dau);
                ret = TRUE;
            }
            set_char_at (buffer, i, db1[c]);
            return ret;
        };

        bool phu_am = false;
        GtkTextIter iter = ret_insert_iter (buffer);
        vector<GtkTextIter> v;
        do{
            string c = ret_char_at (buffer, iter);
            if (db0.count(c) == 0) break;
            else{
                c = db0[c];
                if (regex_match(get(c, 0), regex("[aeiouyAEIOUY]$"))) v.push_back(iter);
                else{
                    if (v.empty()) phu_am = true;
                    else{
                        if (ascii_to_low_case(c) == "q" && ascii_to_low_case(ret_char_at(buffer, v.back())) == "u") v.pop_back();
                        if (ascii_to_low_case(c) == "g" && ascii_to_low_case(ret_char_at(buffer, v.back())) == "i" && v.size()>1) v.pop_back();
                        break;
                    }
                }
            }
        }while (dec(iter));
        if (v.empty()) return FALSE;

        if (v.size() == 1) return dat_dau(v[0]);
        for (auto i = v.begin(); i!=v.end(); i++) {
            if (regex_search(db0[ret_char_at(buffer, *i)], regex(R"(([eE]\^)|([oO]\*))") )) return dat_dau(*i);
        }
        if (v.size() == 3) return dat_dau(v[1]);
        if (v.size() == 2 && phu_am) return dat_dau(v[0]);
        if (v.size() == 2) return dat_dau(v.back());

        return FALSE;
    }else{
        gboolean ret = FALSE;
        GtkTextIter inster_iter = ret_insert_iter (buffer);
        string c = ret_char_at (buffer, inster_iter);
        if (db0.count(c) == 0) return FALSE;
        c = db0[c];
        string dau = [&, key, c]()->string {
            for (auto i = method[config["CURRENT"]].begin(); i!=method[config["CURRENT"]].end(); i++){
                if (i->second == key && get(i->first, 0) == get(c, 0)) return get(i->first, 1);
            }
            return string("");
        }();
        if (dau == "") return FALSE;
        if (get(c, 1)==dau) {
            c = set(c, 1, "");
            ret = FALSE;
        }else{
            c = set(c, 1, dau);
            ret = TRUE;
        }

        set_char_at(buffer, inster_iter, db1[c]);
        return ret;
    }
    
    return FALSE;
}
