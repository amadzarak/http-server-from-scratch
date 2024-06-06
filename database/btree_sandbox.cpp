// resource https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/BTree.html
// B-Trees are attributed to R. Bayer and E. McCreight who described them in 1972.
// By 1979 they had replaced all large-file access methods other than hashing.
// B-trees ARE the standard file organization for applications requiring insertion,
// deletion and key-range searches
//
// A B-tree of order m is defined to have the following shape properties:
// The root is either a leaf or has at least two children
// Each internal node, except for the root, has betwee (m/2) and m children
// All leaves are at the same level
// A B-tree is a generalization of a 2-3 tree

#include <iostream>
#include <utility>

template<class T, int Order>
struct node{
    int number_of_keys;
    int order;
    int position = -1; // to allocate value in the appropriate place
    T * keys = new T[Order];

    node ** children = new node*[Order];

    node(int order);
    int insert(T value);
    node * split(node * node, T * value);
    void print();
    void print_util(int height, bool check_parent);
    int get_height();
    ~node();
};


// node implementation

template<class T, int Order>
node<T, Order>::node(int order) {
    this->order = order;
    this->number_of_keys = 0;
}

template<class T, int Order>
int node<T, Order>::insert(T value) {
    if (this->children[0] == NULL) {
        this->keys[++this->position] = value;
        ++this->number_of_keys;

        for (int i = this->position; i>0; i--) {
            if (this->keys[i] < this->keys[i-1]) std::swap(this->keys[i], this->keys[i-1]);
        }
    } else {
        int i = 0;
        for (; i<this->number_of_keys>0 && value > this->keys[i];) {
            i++;
        }

        int check = this->children[i]->insert(value);

        if(check) {
            T mid;
            int temp = i;
            node<T, Order> * new_node = split(this->children[i], &mid);
            for (; i<this->number_of_keys>0 && mid > this->keys[i];) {
                i++;
            }

            for (int j = this->number_of_keys; j > i; j--) this->keys[j] = this->keys[j-1];
            this->keys[i] = mid;

            ++this->number_of_keys;
            ++this->position;

            //allocate new node split in the correct place

            int k;
            for (k = this->number_of_keys; k > temp + 1; k--)
                this->children[k] = this->children[k-1];
            this->children[k] = new_node;
        }
    }
    if(this->number_of_keys == this->order) return 1;
    else return 0;
}

template<class T, int Order>
node<T, Order> * node<T, Order>::split(node *node, T *med) {
    ::node<T, Order> * new_node = new ::node<T, Order>(order);
    int midpoint = number_of_keys / 2;
    * med = node->keys[midpoint];

   int i;

  for (i = midpoint + 1; i < number_of_keys; ++i) {
      new_node->keys[++new_node->position] = node->keys[i];
      new_node->children[new_node->position] = node->children[i];
      ++new_node->number_of_keys;
      --node->position;
      --node->number_of_keys;
      node->children[i] = NULL;
  }
  new_node->children[new_node->position + 1] = node->children[i];
  node->children[i] = NULL;

  --node->number_of_keys;
  --node->position;

  return new_node;

}

template<class T, int Order>
void node<T, Order>::print() {
    int height = this->get_height();
    for (int i = 1; i <= height; ++i) {
        if (i == 1) print_util(i, true);
        else print_util(i, false);
        std::cout << std::endl;
    }

    std::cout << std::endl;

}

template<class T, int Order>
void node<T, Order>::print_util(int height, bool check_root) {
    if (height == 1 || check_root) {
        for (int i = 0; i < this->number_of_keys; i++) {
            if (i == 0) std::cout << "|";
            std::cout << this->keys[i];
            if (i != this->number_of_keys - 1) std::cout << "|";
            if (i == this->number_of_keys - 1) std::cout << "|" << " ";
        }
    } else {
        for (int i = 0; i <= this->number_of_keys; i++) {
            this->children[i]->print_util(height - 1, false);
            std::cout << std::endl;
        }
    }
}

template<class T, int Order>
int node<T, Order>::get_height() {
    int count = 1;
    node<T, Order> * current = this;
    while (true) {
        if (current->children[0] == NULL) {
            return count;
        }
        current = current->children[0];
        count++;
    }
}

template<class T, int Order>
node<T, Order>::~node() {
    delete[]keys;
    for (int i = 0; i <= this->number_of_keys; ++i) {
        delete this->children[i];
    }
}

template<class T, int Order>
class b_tree {
    private:
        node<T, Order> * root;
        int order;
        int count = 0;
    public:
        b_tree();
        void insert(T value);
        void print() const;
        ~b_tree();
};

template <class T, int Order>
b_tree<T, Order>::b_tree() {
    this->order = order;
    this->root = NULL;
}

template<class T, int Order>
void b_tree<T, Order>::insert(T value) {
    count++;
    if (this->root == NULL) {
        this->root = new node<T, Order>(this->order);
        this->root->keys[++this->root->position] = value;
        this->root->number_of_keys = 1;
    } else {
        int check = root->insert(value);
        if (check) {
            T mid;
            node<T, Order> * split_node = this->root->split(this->root, &mid);
            node<T, Order> * new_node = new node<T, Order>(this->order);
            new_node->keys[++new_node->position] = mid;
            new_node->number_of_keys = 1;
            new_node->children[0] = root;
            new_node->children[1] = split_node;
            this->root = new_node;
        }
    }
}

template<class T, int Order>
void b_tree<T, Order>::print() const {
    if (root != NULL)
        root->print();
    else std::cout << "Empty" << std::endl;
}

template<class T, int Order>
b_tree<T, Order>::~b_tree() {
    delete root;
}

int main(int argc, const char * argv[]) {
    b_tree<int, 4> t1;
    t1.insert(1);
    t1.insert(5);
    t1.insert(0);
    t1.insert(4);
    t1.insert(3);
    t1.insert(2);
    t1.print();
    return 0;
}
