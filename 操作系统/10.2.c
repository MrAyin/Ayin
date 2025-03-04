#include <stdio.h>

void max_value(int x, int cc[][2]); // 函数声明，页表处理
int r_algorithm(int cc[][2]); // 函数声明，选择页面淘汰算法
char cc[13]; // Y 表示产生缺页中断，N 表示未产生缺页中断
void page_table(int page1, int c[5][2]); // 打印页表

void main() {
    int i, j, page, row = 0, col = 1; // b[row][col], 行/列指针
    int k = 0; // 记录缺页中断次数
    int a[12] = {4, 3, 2, 1, 4, 3, 5, 4, 3, 2, 1, 5}; // 存放页的调度顺序
    int b[3][13]; // 模拟内存（分配三个页面）
    int c[5][2] = {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}}; // 定义页表并赋初值
    int d[13], p = 0; // 存放页面淘汰顺序，p 页面淘汰数组 d 的指针

    b[0][0] = 0;
    b[1][0] = 0;
    b[2][0] = 0;

    // *************** 页面调度处理 ***************************************
    for (i = 0; i < 12; i++) {
        if (a[i] == b[0][i] || a[i] == b[1][i] || a[i] == b[2][i]) {
            b[0][i + 1] = b[0][i]; // 将前一列数据复制到下一列
            b[1][i + 1] = b[1][i];
            b[2][i + 1] = b[2][i];
            max_value(a[i], c); // 处理页表，a[i]页面是刚被访问的页面
            page_table(a[i], c); // 打印页表
            cc[i] = 'F';
            col++; // col 指向下一列
        } else { // 页面不在内存
            if (row > 2) { // row > 2 表示内存已没有空闲页面
                page = r_algorithm(c); // 返回淘汰的页面 page
                d[p] = page; // d[]存放被淘汰的页面
                p++;
                k++; // 缺页中断次数
                b[0][i + 1] = b[0][i]; // 将前一列数据复制到下一列
                b[1][i + 1] = b[1][i];
                b[2][i + 1] = b[2][i];
                cc[i] = 'Y';
                if (b[0][i + 1] == page)
                    b[0][i + 1] = a[i];
                if (b[1][i + 1] == page)
                    b[1][i + 1] = a[i];
                if (b[2][i + 1] == page)
                    b[2][i + 1] = a[i];
                max_value(a[i], c); // 访问 a[i]页面，i 页面是刚被访问的页面
                page_table(a[i], c); // 打印页表
            } else {
                b[0][i + 1] = b[0][i]; // 将前一列数据复制到下一列
                b[1][i + 1] = b[1][i];
                b[2][i + 1] = b[2][i];
                cc[i] = 'Y';
                b[row][col] = a[i]; // a[i]页面进入内存
                max_value(a[i], c); // 访问 a[i]页面，i 页面是刚被访问的页面
                col++;
                k++; // 缺页中断次数
                row++;
                page_table(a[i], c); // 打印页表
            }
        }
    }

    // ================= 显示处理结果 ===================================
    printf("\n ");
    for (i = 0; i < 12; i++)
        printf("%6d", a[i]); // 显示页面调度顺序
    printf("\n ============================================================\n");
    for (j = 0; j < 13; j++)
        printf("%6d", b[0][j]);
    printf("\n ------------------------------------------------------------\n");
    for (j = 0; j < 13; j++)
        printf("%6d", b[1][j]);
    printf("\n ------------------------------------------------------------\n");
    for (j = 0; j < 13; j++)
        printf("%6d", b[2][j]);
    printf("\n ------------------------------------------------------------\n");
    printf(" ");
    for (j = 0; j < 13; j++) { // 打印是否产生缺页中断
        printf(" ");
        putchar(cc[j]);
    }
    printf("\n 缺页中断次数：%4d\n", k);
    printf("\n 页面淘汰顺序：");
    for (j = 0; j < p; j++)
        printf("%3d", d[j]); // 显示页面淘汰顺序
    printf("\n\n");
}

// ============ 访问的页面在内存的处理（页表处理） ===================
void max_value(int x, int cc[][2]) { // x-页号：求页表中计数的最大值，并将该页面置为最新访问页面
    int i, max;
    max = cc[0][1];
    for (i = 0; i < 5; i++)
        if (max < cc[i][1])
            max = cc[i][1];
    for (i = 0; i < 5; i++)
        if (cc[i][0] == x)
            cc[i][1] = max + 1;
}

// ============ 选择被淘汰的页面（页表处理） ======================
int r_algorithm(int cc[5][2]) {
    int i, min, row, p;
    for (i = 0; i < 5; i++) // 查询第一个计数为非 0 的页面的计数值
        if (cc[i][1] != 0) {
            min = cc[i][1];
            p = cc[i][0];
            row = i;
            break;
        }
    for (i = 0; i < 5; i++) // 寻找计数值最小的页面
        if (min > cc[i][1] && cc[i][1] != 0) {
            min = cc[i][1];
            p = cc[i][0]; // 最小数所对应的页号被淘汰
            row = i; // 记录最小数所在的行
        }
    cc[row][1] = 0; // 在页表中被淘汰的页面计数清零
    return p; // 返回被淘汰的页面-P
}

// ================= 打印页表 ====================================
void page_table(int page1, int c[5][2]) {
    int i;
    printf("页面调度顺序 page = %d\n", page1);
    for (i = 0; i < 5; i++)
        printf("%5d%5d\n", c[i][0], c[i][1]);
}
