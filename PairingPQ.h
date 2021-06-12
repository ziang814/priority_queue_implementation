// Project identifier: 19034C8F3B1196BF8E0C6E1C0F973D2FD550B88F

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the 'priority_queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure to
            // initialize it here.
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr },previous{nullptr}
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;
            
        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            // TODO: Add one extra pointer (parent or previous) as desired.
            Node* previous;
    }; // Node


    // Description: Construct an empty priority_queue with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp },root(nullptr),numOfNode(0) {
        // TODO: Implement this function.
    } // PairingPQ()


    // Description: Construct a priority_queue out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root(nullptr), numOfNode(0) {
        // TODO: Implement this function.

        //push will handle the number of Nodes for us, so we don't need to update numOfNode here
        while (start != end) {
            push(*start);
            start++;
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ& other) :
        BaseClass{ other.compare },root(nullptr),numOfNode(0) {
        // TODO: Implement this function.

        //if other's root is not empty
        
        std::deque<Node*> vic;
        vic.push_back(other.root);
        //we make sure there are no nullptr in the deque
        while (!vic.empty()) {
            if (vic.front()->child) {
                vic.push_back(vic.front()->child);
            }
            if (vic.front()->sibling) {
                vic.push_back(vic.front()->sibling);
            }
            push(vic.front()->elt);
            vic.pop_front();
        }
        
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ& operator=(const PairingPQ& rhs) {
        // TODO: Implement this function.
        PairingPQ vic(rhs);
        std::swap(numOfNode, vic.numOfNode);
        std::swap(root, vic.root);
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        //if root is nullptr, we don't need to do anything
        if (root) {
            std::deque<Node*> vic;
            vic.push_back(root);
            while (!vic.empty()) {
                if (vic.front()->child) {
                    vic.push_back(vic.front()->child);
                }
                if (vic.front()->sibling) {
                    vic.push_back(vic.front()->sibling);
                }
                vic.front()->sibling = nullptr;
                vic.front()->child = nullptr;
                vic.front()->previous = nullptr;
                delete vic.front();
                vic.front() = nullptr;
                vic.pop_front();
            }
        }
    } // ~PairingPQ()
    

    // Description: Assumes that all elements inside the priority_queue are out of order and
    //              'rebuilds' the priority_queue by fixing the priority_queue invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        //if root is nullptr, we don't do anything; also, the numOfNode doesn't change so 
        //we don't do anything to it
        if (root) {
            std::deque<Node*> vic;
            if (root->child) {
                vic.push_back(root->child);
            }
            root->child = nullptr;
            while (!vic.empty()) {
                if (vic.front()->child) {
                    vic.push_back(vic.front()->child);
                }
                if (vic.front()->sibling) {
                    vic.push_back(vic.front()->sibling);
                }
                vic.front()->previous = nullptr;
                vic.front()->child = nullptr;
                vic.front()->sibling = nullptr;
                root = meld(root, vic.front());
                vic.pop_front();
            }
        }
    } // updatePriorities()


    // Description: Add a new element to the priority_queue. This is almost done,
    //              in that you should implement push functionality in the addNode()
    //              function, and this function should call addNode().
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE & val) {
        // TODO: Implement this function.
        
        //check if root is empty, most of time; most of time it will be non empty
        //becasue we won't check if root is empty in addNode
        if (root) {
            addNode(val);
        }
        else {
            root = new Node(val);
            numOfNode++;
        }
        
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the priority_queue.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the priority_queue is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        //we assume pop will not be called on empty heap, so we won't check if the heap
        //is empty here

        //if the root has a child(most of time is this case)
        if (root->child) {
            //if the child has sibling(most of time)
            if (root->child->sibling) {
                std::deque<Node*> candidates;
                //we will make use of multiple pass method here
                Node* cur = root->child;
                while (cur) {
                    candidates.push_back(cur);
                    cur = cur->sibling;
                }
                //cur will point to nullptr at the end, so won't cause double free

                //we meld two by two until we only have one left, and that one will be the new root
                while (candidates.size() > 1) {
                    candidates[0]->previous = nullptr;
                    candidates[0]->sibling = nullptr;
                    candidates[1]->previous = nullptr;
                    candidates[1]->sibling = nullptr;
                    Node* vic = meld(candidates[0],candidates[1]);
                    candidates.push_back(vic);
                    candidates.pop_front();
                    candidates.pop_front();
                }
                //delete root first
                delete root;
                //then set root to the new 
                root = candidates.front();
            }
            //child has no sibling, and it will be the root now
            else {
                Node* vic = root->child;
                delete root;
                root = vic;
            }
        }
        //root doesn't have a child, which is pretty easy to handle with
        else {
            delete root;
            root = nullptr;
        }
        numOfNode--;
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE & top() const {
        // TODO: Implement this function

        return root->elt;
    } // top()


    // Description: Get the number of elements in the priority_queue.
    // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function
        return numOfNode;
    } // size()

    // Description: Return true if the priority_queue is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function
        return numOfNode == 0;
    } // empty()


    // Description: Updates the priority of an element already in the priority_queue by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain priority_queue invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    // TODO: when you implement this function, uncomment the parameter names.
    void updateElt(Node* node, const TYPE & new_value) {
        // TODO: Implement this function
        //edge case, node is root
        if (node == root) {
            node->elt = new_value;
            return;
        }
        //general case, node isn't root

        //update the value of node to new_value
        node->elt = new_value;
        //node is not leftmost(most of time)
        if (node->previous->child != node) {
            //if its sibling isn't nullptr
            if (node->sibling) {
                node->previous->sibling = node->sibling;
                node->sibling->previous = node->previous;
                node->previous = nullptr;
                node->sibling = nullptr;
                root = meld(root, node);
            }
            else {
                node->previous->sibling = nullptr;
                node->previous = nullptr;
                root = meld(root, node);
            }
        }
        //node is leftmost
        else {
            //if it has sibling
            if (node->sibling) {
                node->previous->child = node->sibling;
                node->sibling->previous = node->previous;
                node->previous = nullptr;
                node->sibling = nullptr;
                root = meld(root, node);
            }
            else {
                node->previous->child = nullptr;
                node->previous = nullptr;
                root = meld(root, node);
            }
        }
    } // updateElt()


    // Description: Add a new element to the priority_queue. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE & val) {
        // TODO: Implement this function
        
        //create a Node* to a node with val on heap
        Node* vic = new Node(val);
        //call meld to incorporate vic into the heap
        //do we need to chcek if root is empty? depends on what functions we will call root
        //for now, just leave it here and later will add conditions to check root = nullptr if needed
        if (root) {
            root = meld(root, vic);
        }
        else {
            root = vic;
        }
        numOfNode++;
        return vic;
    } // addNode()

    

private:
    // TODO: Add any additional member functions or data you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).

    //in the function calling meld we have already checked that both a and b have no previous, sibling and may or may not 
    //have child, and a and b are not equal to each other; also, be sure always pass in  non-nullptr a and b so that we don't
    //have to check if a or b is nullptr
    Node* meld(Node* a, Node* b) {
        //for now, we will just check all conditions, and see if we can optimize after implementing other functions
        //if a and b both are not nullptr
        
        //if b has higher priority, we will make a child of b
        if (this->compare(a->getElt(), b->getElt())) {
            //if b has a child
            if (b->child) {
                a->previous = b;
                b->child->previous = a;
                a->sibling = b->child;
                b->child = a;
            }
            //b doesn't have a child
            else {
                a->previous = b;
                b->child = a;
            }
            return b;
        }
        //a's priority is higher than b's
        else {
            if (a->child) {
                b->previous = a;
                a->child->previous = b;
                b->sibling = a->child;
                a->child = b;
            }
            //b doesn't have a child
            else {
                b->previous = a;
                a->child = b;
            }
            return a;
        }
    }
    Node* root;
    size_t numOfNode;

};


#endif // PAIRINGPQ_H
