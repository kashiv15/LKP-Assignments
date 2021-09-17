#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

static char *int_str;

/* I read the article https://tldp.org/LDP/lkmpg/2.6/html/lkmpg.html#AEN121 */
/* [X1: point 1]
 * Explain following in here.
 * MODULE_LICENSE("GPL") indicates that code is licensed under GPL and not open-source
 * MODULE_DESCRIPTION() is used to describe what the module does
 * MODULE_AUTHOR() declares the module's author */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("[Kashi Vishwanath]");
MODULE_DESCRIPTION("LKP Exercise 3");

/* [X2: point 1]
 * Explain following in here.
 * module_param() macro takes 3 arguments: the name of the variable-int_str, its type- char pointer and permissions- S_IRUSR(read permission bit for owner of file), S_IRGRP(read permission bit for group owner of file) and S_IROTH(read  permission bit for other users).
 */
module_param(int_str, charp, S_IRUSR | S_IRGRP | S_IROTH);

/* [X3: point 1]
 * Explain following in here.
 * arguments that the module can take and a sting to describe.
 */
MODULE_PARM_DESC(int_str, "A comma-separated list of integers");

/* [X4: point 1]
 * Explain following in here.
 * variable to represent the start (head) of our list 
 */
static LIST_HEAD(mylist);

/* [X5: point 1]
 * Explain following in here.
 * structure to represent a link. struct list_head list is to link each element of type
 * struct entry to others.
 */
struct entry {
	int val;
	struct list_head list;
};

static int store_value(int val)
{       
	struct entry *temp_node = NULL;
	temp_node = kmalloc(sizeof(struct entry), GFP_KERNEL);
	if(temp_node == NULL){return -ENOMEM;}
	temp_node->val = val;
	INIT_LIST_HEAD(&temp_node->list);
	list_add_tail(&temp_node->list, &mylist);
	return 0;
	/* [X6: point 10]
	 * Allocate a struct entry of which val is val
	 * and add it to the tail of mylist.
	 * Return 0 if everything is successful.
	 * Otherwise (e.g., memory allocation failure),
	 * return corresponding error code in error.h (e.g., -ENOMEM).
	 */
}

static void test_linked_list(void)
{
	struct entry *temp;
        int count = 0;
        printk(KERN_INFO "Traversing List \n");

        /*Traversing Linked List and Print its Members*/
        list_for_each_entry(temp, &mylist, list) {
        printk(KERN_INFO "Node %d data = %d\n", count++, temp->val);
        }

        printk(KERN_INFO "Total Nodes = %d\n", count);
	/* [X7: point 10]
	 * Print out value of all entries in mylist.
	 */
}


static void destroy_linked_list_and_free(void)
{
	        /* Go through the list and free the memory. */
        struct entry *cursor, *temp;
        list_for_each_entry_safe(cursor, temp, &mylist, list) {
            list_del(&cursor->list);
            kfree(cursor);
        }
	/* [X8: point 10]
	 * Free all entries in mylist.
	 */
}


static int parse_params(void)
{
	int val, err = 0;
	char *p, *orig, *params;


	/* [X9: point 1]
	 * Explain following in here.
	 * kstrdup make int_str to duplicate, GFP_KERNEL is mask used when allocating memory. And check if params is empty. 
	 */
	params = kstrdup(int_str, GFP_KERNEL);
	if (!params)
		return -ENOMEM;
	orig = params;

	/* [X10: point 1]
	 * Explain following in here.
	 * while loops over all the integers given by the user delimited by a comma.
	 * kstrtoint converts string to integer. 	 
	 */
	while ((p = strsep(&params, ",")) != NULL) {
		if (!*p)
			continue;
		/* [X11: point 1]
		 * Explain following in here.
		 */
		err = kstrtoint(p, 0, &val);
		if (err)
			break;

		/* [X12: point 1]
		 * Explain following in here.
		 * calls the function to add elements to the end of tail. return is either 0(success) or EINMEM if memory error.
		 */
		err = store_value(val);
		if (err)
			break;
	}

	/* [X13: point 1]
	 * Explain following in here.
	 * kfree frees previously allocated params from the memory.
	 */
	kfree(orig);
	return err;
}

static void run_tests(void)
{
	/* [X14: point 1]
	 * Explain following in here.
	 * calls the function to traverse the list.
	 */
	test_linked_list();
}

static void cleanup(void)
{
	/* [X15: point 1]
	 * Explain following in here.
	 * frees the list elements after traversing the list function is executed.
	 */
	printk(KERN_INFO "\nCleaning up...\n");

	destroy_linked_list_and_free();
}

static int __init ex3_init(void)
{
	int err = 0;

	/* [X16: point 1]
	 * Explain following in here.
	 * if no arguments are given while loading the module prints some info and returns.
	 */
	if (!int_str) {
		printk(KERN_INFO "Missing \'int_str\' parameter, exiting\n");
		return -1;
	}

	/* [X17: point 1]
	 * Explain following in here.
	 * this function call duplicates the given arguments and calls store_val to add elements to the tail of the list.
	 * if no error the continue with normal execution if error call cleanup() to free 
	 */
	err = parse_params();
	if (err)
		goto out;

	/* [X18: point 1]
	 * Explain following in here.
	 * call the function to traverse through the list.
	 */
	run_tests();
out:
	/* [X19: point 1]
	 * Explain following in here.
	 * this is called when error in parse_params functions to clean the linked list memory.
	 */
	cleanup();
	return err;
}

static void __exit ex3_exit(void)
{
	/* [X20: point 1]
	 * Explain following in here.
	 * this function is called while exiting from module. Below is added by me
	 */
	printk(KERN_INFO "Module exiting....\n");
	return;
}

/* [X21: point 1]
 * Explain following in here.
 * ex3_init is called while loading the module
 */
module_init(ex3_init);

/* [X22: point 1]
 * Explain following in here.
 * ex3_exit is called while unloading the module.
 */
module_exit(ex3_exit);
