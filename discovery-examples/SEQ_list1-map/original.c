/*
  This is one of several examples making use of a user-defined single-linked list.
  In this example, we create a list, populate it with a few numbers and then increment
  each element by one using a generic traversal function, which applies the "increment"
  function to each list element. Effectively, this is a map skeleton, but with the added
  complication that we're dealing with a linked list.

  Depending on its use an advanced analysis could possibly determine that the linked list
  is not really required at all, but could be replaced with another STL data structure/
  container and the map could be parallelised, which in this current version is prevented
  by the linked list data structure.

  At least, we could parallelise the traversal loop in the traverse function using decoupled
  software pipelining, but it would be better to recognise it as a map and replace the list
  data structure with something more "parallel". This means we would need to know how the
  data structure is used and modified and where these uses and modifications happen.
 */

#include <stdio.h>
#include <stdlib.h>

/* New data type for single-linked lists */
typedef struct node{
  int data;
  struct node* next;
} node;


/* Callback function type for list manipulation */
typedef void (*callback)(node* data);


/* Helper function for creating a new list node */
node* create(int data,node* next)
{
  node* new_node = (node*)malloc(sizeof(node));
  if(new_node == NULL)
    {
      printf("Error creating a new node.\n");
      exit(0);
    }
  new_node->data = data;
  new_node->next = next;
 
  return new_node;
}


/* Another helper function for inserting a new node */
node* prepend(node* head,int data)
{
  node* new_node = create(data,head);
  head = new_node;
  return head;
}


/* Appending a new node */
node* append(node* head, int data)
{
  /* go to the last node */
  node *cursor = head;
  while(cursor->next != NULL)
    cursor = cursor->next;
 
  /* create a new node */
  node* new_node =  create(data,NULL);
  cursor->next = new_node;
 
  return head;
}


/*
    insert a new node after the prev node
*/
node* insert_after(node *head, int data, node* prev)
{
  /* find the prev node, starting from the first node*/
  node *cursor = head;
  while(cursor != prev)
    cursor = cursor->next;
 
  if(cursor != NULL)
    {
      node* new_node = create(data,cursor->next);
      cursor->next = new_node;
      return head;
    }
  else
    {
      return NULL;
    }
}


node* insert_before(node *head, int data, node* nxt)
{
  if(nxt == NULL || head == NULL)
    return NULL;
 
  if(head == nxt)
    {
      head = prepend(head,data);
      return head;
    }
 
  /* find the prev node, starting from the first node*/
  node *cursor = head;
  while(cursor != NULL)
    {
      if(cursor->next == nxt)
	break;
      cursor = cursor->next;
    }
 
  if(cursor != NULL)
    {
      node* new_node = create(data,cursor->next);
      cursor->next = new_node;
      return head;
    }
  else
    {
      return NULL;
    }
}


/* A list traversal function */
void traverse(node* head,callback f)
{
  node* cursor = head;
  while(cursor != NULL)
    {
      f(cursor);
      cursor = cursor->next;
    }
}


/* Count elements of the list */
int count(node *head)
{
  node *cursor = head;
  int c = 0;
  while(cursor != NULL)
    {
      c++;
      cursor = cursor->next;
    }
  return c;
}


/* Search a node in the list */
node* search(node* head,int data)
{
 
  node *cursor = head;
  while(cursor!=NULL)
    {
      if(cursor->data == data)
	return cursor;
      cursor = cursor->next;
    }
  return NULL;
}


/* Sort the list */
node* insertion_sort(node* head)
{
  node *x, *y, *e;
 
  x = head;
  head = NULL;
 
  while(x != NULL)
    {
      e = x;
      x = x->next;
      if (head != NULL)
        {
	  if(e->data > head->data)
            {
	      y = head;
	      while ((y->next != NULL) && (e->data> y->next->data))
                {
		  y = y->next;
                }
	      e->next = y->next;
	      y->next = e;
            }
	  else
            {
	      e->next = head;
	      head = e ;
            }
        }
      else
        {
	  e->next = NULL;
	  head = e ;
        }
    }
  return head;
}


/* Delete the list */
node* reverse(node* head)
{
  node* prev    = NULL;
  node* current = head;
  node* next;
  while (current != NULL)
    {
      next  = current->next;
      current->next = prev;
      prev = current;
      current = next;
    }
  head = prev;
  return head;
}


/* Delete a node from the front of the list */
node* remove_front(node* head)
{
  if(head == NULL)
    return NULL;
  node *front = head;
  head = head->next;
  front->next = NULL;
  /* is this the last node in the list */
  if(front == head)
    head = NULL;
  free(front);
  return head;
}


/* Delete a node from the back of the list */
node* remove_back(node* head)
{
  if(head == NULL)
    return NULL;
 
  node *cursor = head;
  node *back = NULL;
  while(cursor->next != NULL)
    {
      back = cursor;
      cursor = cursor->next;
    }
  if(back != NULL)
    back->next = NULL;
 
  /* if this is the last node in the list*/
  if(cursor == head)
    head = NULL;
 
  free(cursor);
 
  return head;
}


/* Remove any node from the list */
node* remove_any(node* head,node* nd)
{
  /* if the node is the first node */
  if(nd == head)
    {
      head = remove_front(head);
      return head;
    }
 
  /* if the node is the last node */
  if(nd->next == NULL)
    {
      head = remove_back(head);
      return head;
    }
 
  /* if the node is in the middle */
  node* cursor = head;
  while(cursor != NULL)
    {
      if(cursor->next == nd)
	break;
      cursor = cursor->next;
    }
 
  if(cursor != NULL)
    {
      node* tmp = cursor->next;
      cursor->next = tmp->next;
      tmp->next = NULL;
      free(tmp);
    }
  return head;
}


/* Dispose the entire list */
void dispose(node *head)
{
  node *cursor, *tmp;
 
  if(head != NULL)
    {
      cursor = head->next;
      head->next = NULL;
      while(cursor != NULL)
        {
	  tmp = cursor->next;
	  free(cursor);
	  cursor = tmp;
        }
    }
}


/*
    display a node
*/
void display(node* n)
{
  if(n != NULL)
    printf("%d ", n->data);
}


/* Plus 1 */
void increment(node *n)
{
  n->data = n->data + 1;
}


int main(int argc, char *argv[])
{
  /* Our list pointer */
  node* head = NULL;

  callback disp = display;
  callback inc = increment;

  /* Append a few numbers */
  for(int i = 0; i < 10; i++) {
    int data = i;
    head = prepend(head, data);
  }

  /* Increment each element by 1 */
  traverse(head, inc);
  /* The annotation here should be that this is actually a map: map(inc(head)) */
  /* Further annotation: The use of the linked list data structure is compatible with
     an ordered container. We could replace it with a better implementation providing
     more opportunity for effecient parallelisation. */

  /* Now print the list */
  traverse(head, disp);

  return EXIT_SUCCESS;
}
