#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <sys/mman.h>


// int main(int argc, char * argv[])
// {
//     pid_t p = fork();
//     // if(argc > 1) p = -1;
//     if(p < 0){
//         perror("P done fucked up.");
//         return 2;
//     }
//     pid_t q = fork();
//     if(q < 0){
//         perror("Q done fucked up.");
//         return 4;
//     }
//     if(p == 0){
//         if(!q) execlp("/Users/abhay/Code/OS/first_fork", "/Users/abhay/Code/OS/first_fork", NULL);
//         // else 
//     }
//     if(p > 0){
//         printf("Parent.");
//         int n = 0;
//         wait(&n);
//         printf("%d\n", n);
//     }
//     return 0;
// }


#include <iostream>
#include <utility>
#include <queue>

using namespace std;

//just an int, so no functions
class node{
    public:
    int n;
    node * p;//parent
    node * c;//highest order child
    node * r;//right sibling
    node(int k);
};

class Heap;

node::node(int k)
{
    n = k;
    p = nullptr;
    c = nullptr;
    r = nullptr;
}

class Tree{
    public:
    node * root;
    Tree * next;
    Tree * prev;

    int order;
    pair<Heap *, int> extract();
    int peek();
    void merge(Tree * t);  //calling convention : argument root must have higher value than object root.
    Tree(node * r, int o);
    Tree();
    Tree(node * r, node * p);
    void depth();

    void print();
};

void Tree::print()
{
    // node * current = root;
    // while(current){
    //     node * right = current;
    //     while(right){
    //         cout << right->n << ", ";
    //         right = right->r;
    //     }
    //     if(current->p->r && current->p->r->c) current = current->p->r->c;
    //     cout << endl;
    //     current = current->c;
    // }

    queue<node *> q;
    q.push(root);
    // int p = 0, z = 0;
    while(q.size()){
        node * x = q.front();
        q.pop();
        // p++;
        node * y = x->c;
        while(y){
            q.push(y);
            y = y->r;
        }

        cout << x->n << ", ";

    }

    cout << endl;

}

Tree::Tree()
{
    next = nullptr;
    prev = nullptr;
}

// Tree::Tree(node * r, node * n){
//     root = r;
//     next = n;
//     prev = r->r;
//     root->p = nullptr;
//     root->r = nullptr;
// }

Tree::Tree(node * r, int o)
{
    // cout << "." << endl;
    root = r;
    root->r = root->p = nullptr;
    order = o;
    next = nullptr;
    prev = nullptr;
}

void Tree::depth()
{
    node * current = root;
    int t = 0;
    while(current){
        for(int i = 0; i < t; i++) cout << "    ";
        cout << current->n << endl;
        if(current->c){
            t++;
            current = current->c;
            while(current->c){
                for(int i = 0; i < t; i++) cout << "    ";
                cout << current->n << endl;
                t++;
                current = current->c;
            }
            for(int i = 0; i < t; i++) cout << "    ";
                cout << current->n << endl;
        }

        if(!current->p) return;

            // cout << "Break." << endl;
        while(!current->p->r){
            current = current->p;
            t--;
            if(current == root) return;
        }
        t--;
                // t--;
        current = current->p->r;
    }

}

int Tree::peek()
{
    return root->n;
}

void Tree::merge(Tree * t)
{
    if(order != t->order)
        return;
    // cout << "Enter tmerge." << endl;
    //root of new tree becomes highest order child of old tree
    node * old = root->c;
    root->c = t->root;
    t->root->p = root;
    t->root->r = old;

    order++;
}

class Heap{
    public:
    Tree * head; //stored as doubly linked list, ascending order
    Tree * tail; //simplifies matters
    Tree * merge(Tree * t); //calls tree-merge. Makes sure the order is correct.
    void insert(int k); //then calls heap-merge on successive trees

    int extract(); //multiple peeks on trees, then extract on min
    void combine(Heap * h);
    void start();
    void print();
    void attach(Tree * t);
    void depth();

    Heap(){
        head = nullptr;
        tail = nullptr;
    }
    // Heap(Tree * t){
    //     if(t->order){
    //         node * root = t->root->c;
    //         node * nex = root->r;
    //         Tree * current = new Tree(root, nullptr);
    //         tail = head = root;
    //         // node * pre = root;
    //         root = nex;
    //         while(root){
    //             nex = root->r;
    //             current = new Tree(root, head);
    //             root = nex;
    //         }
    //     }
    // }

};

void Heap::print()
{
    if(!head){
        cout << "Empty." << endl;
        return;
    }
    Tree * current = head;
    while(current){
        cout << current->order << ": " << endl;
        current->print();
        current = current->next;
        cout << endl << endl;
    }
}

pair<Heap *, int> Tree::extract()
{
    int out = peek();
    if(!order) return {nullptr, out};
    // Tree * dummy = new Tree();
    Heap * h = new Heap();
    // h->head = h->tail = dummy;
    node * current = root->c;

    
    // if(current) 
    root->c = current->r;
    // else root

    Tree * t  = new Tree(current, order - 1);
    // cout << "New tree." << endl;
    // t->print();
    h->tail = h->head = t;
    // cout << "New heap." << endl;
    // h->print();

    current = root->c;
    int o = order - 2;
    while(current){
        root->c = current->r;
        t = new Tree(current, o);
        // cout << "New tree." << endl;
        // cout << o << ": " << endl;
        // t->print();
        o--;
        h->attach(t);
        // cout << "New heap." << endl;
        // h->print();
    // cout << ":after attaching." << endl;
        current = root->c;
    }


    return {h, out};
}

Tree * Heap::merge(Tree * t)
{
    if(!t) return nullptr;
    // cout << "Enter hmerge." << endl;
    Tree * u = t->next;
    if(!u) return nullptr;
    if(t->order != u->order) return nullptr;

    //swapping trees
    if(t->peek() > u->peek()){
        node * tm = t->root;
        t->root = u->root;
        u->root = tm;
    }
    // cout << "Swapped." << endl;
    //now, t.root has the lesser value.

    // while(t->order == u->order){
        t->merge(u);
        t->next = u->next;
        if(u->next) u->next->prev = t;
        else tail = t;

        // u = t->next;
    // }

    // cout << "MErged." << endl;


    return t;
    // cout << "Exit hmerge." << endl;
}

void Heap::insert(int k)
{
    node * add = new node(k);
    Tree * t = new Tree(add, 0);
    if(head == nullptr || tail == nullptr){
        head = tail = t;
        cout << "Init." << endl;
        return;
    }
    else{
        head->prev = t;
        t->next = head;
        head = t;
    }

    Tree * current = head;
    while(current){
        current = merge(current);
        // cout << "Cur :" << endl;
        // current->print();
        // cout << endl;
        // current = current->next;
    }
    // cout << endl;

}

void Heap::combine(Heap * h)
{
    // h->print();
    // cout << "------" << endl;
    // print();
    if(!h) return;
    Tree * c1 = head;
    if(!c1){
        head = h->head;
        tail = h->tail;
        return;
    }
    Tree * c2 = h->head;

    //all trees of new heap are merged with trees from original heap. Heap is restored parallely.
    // while(c2 != nullptr){
    //     if(c1->root->n < c2->root->n){
    //         node * tm = c1->root;
    //         c1->root = c2->root;
    //         c2->root = tm;
    //     }
    //     //merges with tree from new heap
    //     c1->merge(c2);
    //     //merges with conflicting tree from current heap
    //     Tree * c = merge(c1);
    //     if(c) c1 = c;
    //     c1 = c1->next;
    //     c2 = c2->next;
    // }

    while(c2){
        Tree * c = c2->next;
        // cout << "COMB." << endl;
        attach(c2);
        // print();
        c2 = c;
    }
}

int Heap::extract()
{
    Tree * current = head;
    Tree * min = current;
    if(!head)
        return -1;
    

    // cout << "Tree_ext." << endl;
    if(head == tail){
        if(min->order == 0){
            // cout << "Heap emptied." << endl;
            int out = min->peek();
            delete min;
            head = tail = nullptr;
            return out;
        }
        pair<Heap *, int> p = min->extract();
        // cout << "h=t" << endl;
        head = p.first->head;
        tail = p.first->tail;
        // delete min;
        return p.second;
    }

    while(current){
        if(current->peek() < min->peek()) min = current;

        current = current->next;
    }
    // cout << "Min found: " << min->peek()  << endl;

    // delete min

    if(min->prev) min->prev->next = min->next;
    if(min->next) min->next->prev = min->prev;

    if(min == head) head = head->next;
    

    if(min == tail) tail = tail->prev;


    // cout << ": after deleting min" << endl;
    // print();


    pair<Heap *, int> p = min->extract();

    // cout << "New heap."<< endl;
    int out = get<1>(p);

// cout << ":This." << endl;
    // print();
// cout << ":Not this." << endl;
    // p.first->print();

    if(p.first) combine(p.first);

    return out;
}

void Heap::attach(Tree * t)
{
    Tree * current = head;
    // cout << "Attach." << endl;
    // print();
    while(current){
        if(current->order == t->order){
            if(current->next) current->next->prev = t;
            t->next = current->next;
            current->next = t;
            t->prev = current;

            // cout << "t->print()" << endl;
            // t->print();
            while(t && current->order == t->order){
                current = merge(current);
                t = current->next;
            }
            // cout << "current->print()" << endl;
            // current->print();

            break;
            // continue;
        }
        if(current->order > t->order){
            if(current == head) head = t;
            if(current->prev) current->prev->next = t;
            t->next = current;
            t->prev = current->prev;
            current->prev = t;

            // if(head == current) head = t;
            break;
        }

        current = current->next;
    }

    if(!current){
        tail->next = t;
        t->prev = tail;
        tail = t;
    }
}

void Heap::start()
{
    std::cout << "Following are the acceptable commands : \n\n"
    "add <value> : adds node to heap with given value.\n"
    "ext : prints minimum value.\n"
    "show : inorder traversal.\n"
    "tree: displays structure.\n"
    "spawn : creates new heap. Merges it to original after exit.\n"
    "exit : terminates program.\n\n";

    string cmd;
    while(true){
        cout << "Enter command.\n\n>>> ";
        cin >> cmd;
        if(cmd == "add"){
            int val;
            cin >> val;
            insert(val);

        }
        else if(cmd == "spawn"){
            

            pid_t p = fork();
            if(p < 0) cout << "Spawn failed." << endl;
            if(p == 0){
                execlp("/Users/abhay/Code/OS/quad", "/Users/abhay/Code/OS/quad",".", NULL);
            }
            else{
                // int * n = (int *)malloc(sizeof(int));
                wait(NULL);
                // // *n >>= 8;
                // // *n -= 0xfd9a0;
                // printf("%p\n", n);
                // Heap * h = (Heap *)(n);
                // if(!h){
                //     cout << "Bzz" << endl;
                //     return;
                // }
                // h->print();
                cout << "Spawn terminated. Initiating assimilation." << endl;

                int fd = shm_open("Heap", O_RDWR, 0666);
                cout << "fd: " << fd << endl;
                char * ptr = (char *)mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* read from the shared memory object */
                // printf("SHM: %s\n", ptr);
                Heap * h;
                sscanf(ptr, "%p", &h);
                printf("%p\n", h);
                h->print();
                cout << "." << endl;
                // cout << (ptr == nullptr) << endl;
                shm_unlink("Heap");
                combine((Heap *)h);
            }
        }
        else if(cmd == "ch"){
            int val;
            cin >> val;
            for(int i = 0; i < val; i++){
                int k;
                cin >> k;
                insert(k);
            }
        }
        else if(cmd == "tree"){
            depth();
            cout << endl;
        }
        else if(cmd == "show"){
            std::cout << std::endl;
            print();
            std::cout << std::endl;
        }
        else if(cmd == "ext"){
            int val = extract();
            if(val == -1){
                cout << "Empty." << endl;
                continue;
            }
            cout << val << endl;
        }
        /*
        else if(cmd == "is"){
            int val;
            cin >> val;
            std::cout << std::endl;
            node * is = get<0>(search(val));
            if(is) cout << "YES";
            else cout << "NO";
            std::cout << std::endl;
        }
        */
       else if(cmd == "exit") return;
        else{
            std::cout << "Invalid command." << std::endl << std::endl;
        }
    }

}

void Heap::depth(){
    Tree * current = head;
    while(current){
        cout << current->order << ": \n";
        current->depth();
        current = current->next;
        cout << endl;
    }
}

long long hexer(char * s){
    long long out = 0;
    if(s[1] == 'x' || s[1] == 'X') s += 2;
    int i = 36;
    while(*s){
        out += (*s - '0') * 1 << i;
        i -= 4;
        s++;
    }

    return out;
}

int main(int argc, char * argv[])
{
    Heap * h;
    h = new Heap();
    printf("%p\n", h);
    // printf("%p\n", h);
    h->start();

    
    if(argc > 1)
    {int fd = shm_open("Heap", O_CREAT | O_RDWR, 0666);
    /* configure the size of the shared memory object */ 
    ftruncate(fd, 1024);
   /* memory map the shared memory object */
   char * ptr = (char *)
    mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    sprintf(ptr, "%p", h);
    // printf("%s", ptr);
    // ptr += strlen("Hello.");
    }

    return 0;
}
