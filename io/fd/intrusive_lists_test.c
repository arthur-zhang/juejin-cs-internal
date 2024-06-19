#include <stdio.h>

struct list_head {
    struct list_head *next, *prev;
};
struct my_item {
    long data;
    struct list_head list;
};

// 在链表中插入一个新节点
void list_add(struct list_head *new, struct list_head *prev, struct list_head *next) {
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

// 在链表头部添加一个新节点
void list_add_head(struct list_head *new, struct list_head *head) {
    list_add(new, head, head->next);
}

int main() {

    // 初始化链表
    struct list_head list;
    list.next = &list;
    list.prev = &list;

    struct my_item item1, item2, item3;
    item1.data = 100;
    item2.data = 200;
    item3.data = 300;
    // 将 item1 添加到链表头部  head -> item1 -> head
    list_add_head(&item1.list, &list);
    // 将 item2 添加到链表头部, head -> item2 -> item1 -> head
    list_add_head(&item2.list, &list);
    // 将 item3 添加到链表头部, head -> item3 -> item2 -> item1 -> head
    list_add_head(&item3.list, &list);

    struct list_head *iterator;
    // 遍历链表
    for (iterator = list.next; iterator != &list; iterator = iterator->next) {
        // 通过链表节点获取数据, 由于链表节点的前8个字节是 data，类型为 long, 所以需要减去8
        struct my_item *item = (struct my_item *) ((char *) iterator - 8);
        printf("Data: %ld\n", item->data);
    }
}