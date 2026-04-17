#include <bits/stdc++.h>
using namespace std;

// ---------- HELPER ----------
string toLower(string s) {
    for(char &c : s) c = tolower(c);
    return s;
}

// ---------------- POST ----------------
struct Post {
    int id;
    string contentType;
    int likes, comments, shares;
    string hashtag;

    int engagement() const {
        return likes + comments + shares;
    }
};

// ---------------- LINKED LIST ----------------
struct Node {
    Post post;
    Node* next;
    Node(Post p) : post(p), next(NULL) {}
};

class LinkedList {
    Node* head;
public:
    LinkedList() : head(NULL) {}

    void add(Post p) {
        Node* n = new Node(p);
        n->next = head;
        head = n;
    }

    vector<Post> getAll() {
        vector<Post> v;
        Node* t = head;
        while(t) {
            v.push_back(t->post);
            t = t->next;
        }
        return v;
    }
};

// ---------------- HEAP ----------------
struct MaxHeap {
    bool operator()(Post a, Post b) {
        return a.engagement() < b.engagement();
    }
};
struct MinHeap {
    bool operator()(Post a, Post b) {
        return a.engagement() > b.engagement();
    }
};

// ---------------- TRIE ----------------
struct TrieNode {
    bool end;
    unordered_map<char, TrieNode*> child;
    TrieNode() : end(false) {}
};

class HashtagTrie {
    TrieNode* root;

    void dfs(TrieNode* node, string s) {
        if(node->end) cout << s << endl;
        for(auto &p : node->child)
            dfs(p.second, s + p.first);
    }

public:
    HashtagTrie() { root = new TrieNode(); }

    string normalize(string s) {
        s = toLower(s);
        if(!s.empty() && s[0] == '#') s = s.substr(1);
        return s;
    }

    void insert(string s) {
        s = normalize(s);
        TrieNode* cur = root;
        for(char c : s) {
            if(!cur->child[c]) cur->child[c] = new TrieNode();
            cur = cur->child[c];
        }
        cur->end = true;
    }

    void suggest(string p) {
        p = normalize(p);
        TrieNode* cur = root;
        for(char c : p) {
            if(!cur->child[c]) {
                cout << "No suggestions found.\n";
                return;
            }
            cur = cur->child[c];
        }
        cout << "Suggested hashtags:\n";
        dfs(cur, p);
    }
};

// ---------------- QUEUE ----------------
class ActivityWindow {
    queue<int> q;
    int k, sum;
public:
    ActivityWindow(int k) : k(k), sum(0) {}

    void add(int v) {
        q.push(v);
        sum += v;
        if(q.size() > k) {
            sum -= q.front();
            q.pop();
        }
    }

    double avg() {
        return q.empty() ? 0 : (double)sum / q.size();
    }
};

// ---------------- TREE (FOLLOWERS) ----------------
class FollowerGrowth {
    map<int,int> mp;
public:
    void record(int day, int f) {
        mp[day] = f;
    }

    void check(int d1, int d2) {
        if(!mp.count(d1) || !mp.count(d2)) {
            cout << "Record not found for the given days.\n";
            return;
        }

        int f1 = mp[d1], f2 = mp[d2];
        cout << "Day " << d1 << " Followers: " << f1 << endl;
        cout << "Day " << d2 << " Followers: " << f2 << endl;

        if(f2 > f1)
            cout << "Follower growth increased. Maintain consistency.\n";
        else if(f2 < f1)
            cout << "Follower growth decreased. Increase posting frequency and use effective hashtags.\n";
        else
            cout << "No change in follower growth.\n";
    }
};

// ---------------- SORTING ----------------
void rankContent(vector<Post> posts) {
    unordered_map<string,int> mp;
    for(auto &p : posts)
        mp[p.contentType] += p.engagement();

    vector<pair<string,int>> v(mp.begin(), mp.end());
    sort(v.begin(), v.end(),
         [](auto &a, auto &b){ return a.second > b.second; });

    cout << "\n--- CONTENT TYPE RANKING ---\n";
    for(auto &p : v)
        cout << p.first << " : " << p.second << endl;

    if(!v.empty())
        cout << "\nInsight: You receive higher engagement from "
             << v[0].first
             << ". Focus more on this content type.\n";
}

// ---------------- MAIN ----------------
int main() {
    LinkedList reels, posts, stories;
    HashtagTrie trie;
    ActivityWindow activity(5);
    FollowerGrowth growth;

    int id = 1;

    while(true) {
        cout << "\n====== SOCIAL MEDIA ASSISTANT ======\n";
        cout << "1. Add Posts\n2. View Timeline\n3. Top Performing Posts\n4. Low Performing Posts\n";
        cout << "5. Content Type Ranking\n6. Hashtag Suggestions\n7. Activity Average\n";
        cout << "8. Record Followers\n9. Check Follower Growth\n0. Exit\n";
        cout << "Enter choice: ";

        int choice;
        while(!(cin >> choice) || choice < 0 || choice > 9) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Please enter a number between 0 and 9: ";
        }

        if(choice == 0) break;

        // -------- ADD POSTS --------
        if(choice == 1) {
            cout << "Enter posts (type 'end' to stop)\n";
            while(true) {
                Post p;
                p.id = id++;

                cout << "Content type (reel/post/story or end): ";
                cin >> p.contentType;
                p.contentType = toLower(p.contentType);

                if(p.contentType == "end") break;

                cout << "Likes: "; cin >> p.likes;
                cout << "Comments: "; cin >> p.comments;
                cout << "Shares: "; cin >> p.shares;
                cout << "Hashtag: "; cin >> p.hashtag;

                if(p.contentType == "reel") reels.add(p);
                else if(p.contentType == "post") posts.add(p);
                else stories.add(p);

                trie.insert(p.hashtag);
                activity.add(p.engagement());
            }
        }

        // -------- VIEW TIMELINE --------
        else if(choice == 2) {
            cout << "Show (reel/post/story/all): ";
            string s; cin >> s;
            s = toLower(s);

            vector<Post> all;
            auto collect = [&](LinkedList &l) {
                auto v = l.getAll();
                all.insert(all.end(), v.begin(), v.end());
            };

            if(s == "reel") collect(reels);
            else if(s == "post") collect(posts);
            else if(s == "story") collect(stories);
            else {
                collect(reels);
                collect(posts);
                collect(stories);
            }

            if(all.empty()) {
                cout << "No posts available.\n";
                continue;
            }

            sort(all.begin(), all.end(),
                 [](Post a, Post b){ return a.id > b.id; });

            cout << "\n--- TIMELINE (LATEST FIRST) ---\n";
            for(auto &p : all) {
                cout << "ID: " << p.id
                     << " | Type: " << p.contentType
                     << " | Engagement: " << p.engagement()
                     << " | Hashtag: " << p.hashtag << endl;
            }
        }

        // -------- TOP / LOW POSTS --------
        else if(choice == 3 || choice == 4) {
            vector<Post> all;
            auto collect = [&](LinkedList &l) {
                auto v = l.getAll();
                all.insert(all.end(), v.begin(), v.end());
            };
            collect(reels); collect(posts); collect(stories);

            if(all.empty()) {
                cout << "No posts available.\n";
                continue;
            }

            if(choice == 3) {
                cout << "\nYour top posts are as follows:\n";
                priority_queue<Post, vector<Post>, MaxHeap> pq(all.begin(), all.end());
                for(int i = 1; i <= min(3, (int)all.size()); i++) {
                    auto p = pq.top(); pq.pop();
                    cout << i << ". Type: " << p.contentType
                         << " | Engagement: " << p.engagement() << endl;
                }
            } else {
                cout << "\nYour low performing posts are as follows:\n";
                priority_queue<Post, vector<Post>, MinHeap> pq(all.begin(), all.end());
                for(int i = 1; i <= min(3, (int)all.size()); i++) {
                    auto p = pq.top(); pq.pop();
                    cout << i << ". Type: " << p.contentType
                         << " | Engagement: " << p.engagement() << endl;
                }
            }
        }

        // -------- CONTENT RANKING --------
        else if(choice == 5) {
            vector<Post> all;
            auto collect = [&](LinkedList &l) {
                auto v = l.getAll();
                all.insert(all.end(), v.begin(), v.end());
            };
            collect(reels); collect(posts); collect(stories);
            rankContent(all);
        }

        // -------- HASHTAG --------
        else if(choice == 6) {
            string p;
            cout << "Enter hashtag prefix: ";
            cin >> p;
            trie.suggest(p);
        }

        // -------- ACTIVITY AVG --------
        else if(choice == 7) {
            double a = activity.avg();
            cout << "Average engagement: " << a << endl;

            if(a < 100)
                cout << "Engagement is low. Improvement is required.\n";
            else if(a < 250)
                cout << "Engagement is moderate. Continue refining your strategy.\n";
            else if(a < 500)
                cout << "Engagement is satisfactory. Keep progressing.\n";
            else if(a < 1000)
                cout << "Engagement is good. Your efforts are yielding results.\n";
            else
                cout << "Engagement is excellent. Your content strategy is highly effective.\n";
        }

        // -------- RECORD FOLLOWERS --------
        else if(choice == 8) {
            cout << "Record follower count (type 'end' to stop)\n";
            while(true) {
                string d;
                cout << "Day: ";
                cin >> d;
                d = toLower(d);

                if(d == "end") break;

                int day = stoi(d);
                int f;
                cout << "Followers: ";
                cin >> f;
                growth.record(day, f);
            }
        }

        // -------- CHECK GROWTH --------
        else if(choice == 9) {
            int d1, d2;
            cout << "From day: "; cin >> d1;
            cout << "To day: "; cin >> d2;
            growth.check(d1, d2);
        }
    }

    cout << "\nExiting Social Media Assistant...\n";
    return 0;
}
