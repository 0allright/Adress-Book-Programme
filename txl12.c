#define _CRT_SECURE_NO_WARNINGS 1 // 关闭使用不安全的CRT函数的警告
#pragma warning(disable:6031)    // 关闭忽略返回值的警告
#include <stdio.h>                 // 包含标准输入输出库
#include <stdlib.h>                // 包含标准库
#include <time.h>
#include <string.h>                // 包含字符串操作函数库
#include <locale.h>                //获取程序的区域设置（locale）
#include <windows.h>              //用于在Windows操作系统上进行系统级编程
#include <time.h>
#include <conio.h>

// 定义联系人结构体
typedef struct {                   // 定义一个结构体来存储联系人信息
	char name[50];                 // 联系人的姓名
	char phone[15];                // 联系人的电话
	char address[100];             // 联系人的地址
	char group[20];                // 联系人的分组
} contact;                         // 结构体命名为contact

#define MAX_CONTACTS 1000        //最多输入1000位联系人
#define LINE_LENGTH 60  // 控制台行长度（就展示这么长）

contact con[MAX_CONTACTS];         // 定义一个contact类型的数组，存储所有联系人
int contact_count = 0;             // 记录当前联系人数量的变量，初始化为0
HANDLE hThread;
HANDLE hEvent;
int keepRunning = 1;

//函数声明
void readfile();                   // 声明读取文件的函数
void writefile();                  // 声明写入文件的函数
void mainmenu();                   // 声明主菜单的函数
void newItem();                    // 声明新增联系人的函数
void show_all();                   // 声明显示所有联系人的函数
void find();                       // 声明查找联系人的函数
void find_all();
void find_byname();                // 声明按姓名查找联系人的函数
void find_bytel();                 // 声明按电话查找联系人的函数
void modify();                     // 声明修改联系人的函数
void modify_byname();              // 声明按姓名修改联系人的函数
void modify_bytel();               // 声明按电话修改联系人的函数
void del();                        // 声明删除联系人的函数
void del_all();                    // 声明删除所有联系人的函数
void del_one();                    // 声明删除单个联系人的函数
void export_contacts();            // 声明导出联系人的函数
void import_contacts();            // 声明导入联系人的函数
void backup_contacts();            // 声明备份联系人的函数
void restore_contacts();           // 声明恢复联系人的函数
void show_by_group();              // 声明按分组显示联系人的函数
int validate_phone(const char* phone); // 声明验证电话的函数
int count_lines(const char* filename);
void print_centered(const char* str);	// 定义一个函数，用于将文本居中打印
void display_developer_info();	// 定义一个函数，用于显示开发者信息
void print_separator();// 定义一个函数，用于打印分隔线
void printCurrentDateTime();// 定义一个函数，用于打印当前日期和时间
void confirm_return();// 定义一个函数，用于确认返回主界面
DWORD WINAPI UpdateTimeThread(LPVOID lpParameter);	// 定义一个函数，用于时间更新线程的入口点
DWORD WINAPI print_time(LPVOID lpParam);
void printCurrentDateTime();// 定义一个函数，用于打印当前日期和时间

void printCurrentDateTime() {
    time_t now;
    time(&now);
    struct tm* local = localtime(&now);
    char currentTime[20];
    strftime(currentTime, sizeof(currentTime), "%Y-%m-%d %H:%M:%S", local);

    COORD pos = { (LINE_LENGTH - strlen(currentTime)) / 2, 30 }; // 设置光标位置为控制台最后一行中央
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf("%-*s\r", LINE_LENGTH, ""); // 用空格覆盖整行
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf("现在是北京时间：%s", currentTime); // 显示新的时间
    fflush(stdout); // 刷新输出缓冲区，确保立即输出
}

DWORD WINAPI print_time(LPVOID lpParam) {
    while (keepRunning) {
        if (WaitForSingleObject(hEvent, 1000) == WAIT_OBJECT_0) {
            break; // 事件触发时退出循环
        }
        printCurrentDateTime();
        Sleep(1000); // 每秒更新一次
    }
    return 0;
}


int main() {
	// 获取当前会话的默认代码页
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, "");
    // 创建事件对象
    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hEvent == NULL) {
        printf("CreateEvent failed (%d)\n", GetLastError());
        return 1;
    }

    // 创建时间显示线程
    hThread = CreateThread(NULL, 0, print_time, NULL, 0, NULL);
    if (hThread == NULL) {
        printf("CreateThread failed (%d)\n", GetLastError());
        CloseHandle(hEvent);
        return 1;
    }


	readfile();                    // 读取文件，加载联系人数据
	mainmenu();                    // 显示主菜单
//	while (1) {
		// 读取用户输入
	//	printf("请选择功能(1-12): ");
		//scanf("%d", &choice);
		// 如果用户选择退出
//		if (choice == 12) {
		// 设置事件，通知时间更新线程停止运行
//			SetEvent(hEvent);
			// 等待时间更新线程结束
	//		WaitForSingleObject(hThread, INFINITE);
			// 关闭线程句柄
		//	CloseHandle(hThread);
			// 清理资源
			//CloseHandle(hMutex);
			//CloseHandle(hEvent);
			//break;
	//	}
		// 根据用户输入执行相应的操作
		// ...
//	}
     // 退出程序前清理资源
    keepRunning = 0;
    SetEvent(hEvent);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);		// 关闭线程句柄
    CloseHandle(hEvent);		// 清理资源

	return 0;
}


//错误事例x2
DWORD WINAPI UpdateTimeThread(LPVOID lpParameter) {// 定义一个函数，用于时间更新线程的入口点
//	char current_time[20];  // 定义一个字符数组，用于存储格式化的日期和时间
//	HANDLE hEvent = (HANDLE)lpParameter;  // 获取传递给线程的参数，即事件句柄
//	while (1) {
//		time_t now = time(0);		// 获取当前时间
//		strftime(current_time, sizeof(current_time), "%Y-%m-%d %H:%M:%S", localtime(&now));
//		printf("现在是北京时间：%s\r", current_time);		// 输出当前时间
//		fflush(stdout);
//		DWORD dwWaitResult = WaitForSingleObject(hEvent, 1000);		// 等待一秒钟或事件被设置
//		if (dwWaitResult == WAIT_OBJECT_0) {// 如果事件被设置，退出线程
//			break;
//		}
	// 如果超时，继续更新时间
    while (keepRunning) {
        if (WaitForSingleObject(hEvent, 1000) == WAIT_OBJECT_0) {
            break; // 事件触发时退出循环
        }
        printCurrentDateTime();
    }
    return 0;
}//错误示例
//DWORD WINAPI UpdateTimeThread(LPVOID lpParameter) {
    // 定义一个字符数组，用于存储格式化的日期和时间
   // char current_time[20];
    // 获取传递给线程的参数，即事件句柄
   // HANDLE hEvent = (HANDLE)lpParameter;
    // 进入无限循环，直到收到停止信号
    //while (1) {
        // 获取当前时间
      //  time_t now = time(0);
        // 将当前时间格式化为字符串
        //strftime(current_time, sizeof(current_time), "%Y-%m-%d %H:%M:%S", localtime(&now));
        // 加锁并更新时间字符串
        // 注意：这里没有使用互斥锁，因为printCurrentDateTime函数中没有共享资源
        // 输出当前时间
        //printf("现在是北京时间：%s\r", current_time);
        // 刷新输出缓冲区，确保立即输出
        //fflush(stdout);
        // 等待一秒钟或事件被设置
        //DWORD dwWaitResult = WaitForSingleObject(hEvent, 1000);
        // 如果事件被设置，退出线程
        //if (dwWaitResult == WAIT_OBJECT_0) {
          //  break;
        //}
        // 如果超时，继续更新时间
    //}
    //return 0;
//}

void readfile() {                  // 读取文件的实现
    fflush(stdout); // 确保提示信息立即输出
    SetEvent(hEvent); // 暂停时间线程

	FILE* file = fopen("D:\\contact.txt", "r");  // 打开contact.txt文件以读取模式
	if (file == NULL) {            // 如果文件不存在
		printf("文件contact.txt不存在，是否创建新文件？ (y/n): ");  // 提示用户文件不存在，是否创建新文件
        fflush(stdout); // 确保提示信息立即输出
        SetEvent(hEvent); // 暂停时间线程

        char choice;               // 定义一个变量来存储用户的选择

		scanf(" %c", &choice);     // 获取用户的选择
        fflush(stdout); // 确保提示信息立即输出
        SetEvent(hEvent); // 暂停时间线程

		if (choice == 'y' || choice == 'Y') {  // 如果用户选择是
			file = fopen("D:\\contact.txt", "w");  // 创建新文件
			if (file == NULL) {    // 如果文件创建失败

				printf("创建文件失败。\n");  // 提示创建失败
                fflush(stdout); // 确保提示信息立即输出
                SetEvent(hEvent); // 暂停时间线程
				exit(1);           // 退出程序，返回1
			}
			fclose(file);          // 关闭文件
		}
		else {                     // 如果用户选择否
			exit(0);               // 退出程序，返回0
		}
	}
	else {                         // 如果文件存在
		while (fscanf(file, "%49s %14s %99s %19s", con[contact_count].name, con[contact_count].phone, con[contact_count].address, con[contact_count].group) != EOF) {  // 读取文件中的联系人信息
			contact_count++;       // 增加联系人计数
		}
		fclose(file);              // 关闭文件
	}
}

void writefile() {                 // 写入文件的实现
	FILE* file = fopen("D:\\contact.txt", "w");  // 打开contact.txt文件以写入模式
	if (file == NULL) {            // 如果文件无法打开
		printf("无法打开文件进行写入。\n");  // 提示无法打开文件
        fflush(stdout); // 确保提示信息立即输出
        SetEvent(hEvent); // 暂停时间线程

		return;                    // 返回，不执行后续操作
	}
	for (int i = 0; i < contact_count; i++) {  // 遍历所有联系人
		fprintf(file, "%s %s %s %s\n", con[i].name, con[i].phone, con[i].address, con[i].group);  // 将联系人信息写入文件
        fflush(stdout); // 确保提示信息立即输出
        SetEvent(hEvent); // 暂停时间线程

	}
	fclose(file);                  // 关闭文件
}

void mainmenu() { // 主菜单的实现
	int choice;                    // 定义变量存储用户选择
//	print_centered("欢迎使用通讯录小程序1.0版本");
	do {                           // 循环显示菜单直到用户选择退出
        system("cls"); // 清屏
//        printCurrentDateTime();
//        print_centered("欢迎使用通讯录小程序1.0版本");
		print_separator();
//		print_centered("通讯录管理系统");
        print_centered("欢迎使用通讯录小程序1.0版本");
//		printf("\n");
        print_separator();
		print_centered("1. 新增联系人");
		print_centered("2. 查询显示联系人");
		print_centered("3. 删除联系人");
		print_centered("4. 修改联系人");
		print_centered("5. 导出联系人");
		print_centered("6. 导入联系人");
		print_centered("7. 备份联系人");
		print_centered("8. 恢复联系人");
		print_centered("9. 显示分组联系人");
		print_centered("10. 显示开发者信息");
		print_centered("11. 保存退出");
		print_centered("12. 不保存退出");
		print_separator();
		print_centered("请选择功能(1-12): ");
        printCurrentDateTime();
        printf("\n");

		if (scanf("%d", &choice) != 1) {// 获取用户选择并检查输入是否有效
			while (getchar() != '\n'); // 清除缓冲区
			printf("无效输入，请输入数字。\n");
			continue; // 继续循环
		}

		switch (choice) {                       // 根据用户选择执行相应操作
			// 定义事件句柄
//			HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//			if (hEvent == NULL) {
				// 处理错误
//				return 1;
//			}
			// 创建线程
//			HANDLE hThread = CreateThread(NULL, 0, UpdateTimeThread, (LPVOID)hEvent, 0, NULL);
//			if (hThread == NULL) {
				// 处理错误
//				CloseHandle(hEvent);
//				return 1;
//		}
		case 1:                                 // 如果选择1
			newItem();                          // 调用新增联系人函数
			confirm_return();                   // 确认返回主界面
			break;                              // 退出switch
		case 2:                                 // 如果选择2
			find();                             // 调用查询联系人函数
			confirm_return();                   // 确认返回主界面
			break;                              // 退出switch
		case 3:                                 // 如果选择3
			del();                              // 调用删除联系人函数
			confirm_return();                   // 确认返回主界面
			break;                              // 退出switch
		case 4:                                 // 如果选择4
			modify();                           // 调用修改联系人函数
			confirm_return();                   // 确认返回主界面
			break;                              // 退出switch
		case 5:                                 // 如果选择5
			export_contacts();                  // 调用导出联系人函数
			confirm_return();                   // 确认返回主界面
			break;                              // 退出switch
		case 6:                                 // 如果选择6
			import_contacts();                  // 调用导入联系人函数
			confirm_return();                   // 确认返回主界面
			break;                              // 退出switch
		case 7:                                 // 如果选择7
			backup_contacts();                  // 调用备份联系人函数
			confirm_return();                   // 确认返回主界面
			break;                              // 退出switch
		case 8:                                 // 如果选择8
			restore_contacts();                 // 调用恢复联系人函数
			confirm_return();                   // 确认返回主界面
			break;                              // 退出switch
		case 9:                                 // 如果选择9
			show_by_group();                    // 调用按分组显示联系人函数
			confirm_return();                   // 确认返回主界面
			break;                              // 退出switch
		case 10:                                // 如果选择10
			display_developer_info();           // 显示开发者信息
			confirm_return();                   // 确认返回主界面
			break;                              // 退出switch
		case 11:                                // 如果选择11
			writefile();                        // 调用写入文件函数
			exit(0);                            // 退出程序，返回0
		case 12:
            
			// 设置事件，通知时间更新线程停止运行
//     		if (!SetEvent(hEvent)) {
				// 如果SetEvent失败，打印错误信息
	//			printf("SetEvent failed with error %d\n", GetLastError());
				// 可以选择退出程序或者进行其他错误处理
	//		}
			// 等待时间更新线程结束
//			WaitForSingleObject(hThread, INFINITE);
			// 关闭线程句柄
//			CloseHandle(hThread);
			// 关闭事件句柄
	//		CloseHandle(hEvent);
			// 退出程序
			exit(0);
		default:                                // 如果选择无效
            print_centered("无效选择，请重新输入。\n");  // 提示用户重新输入
		}
	} while (choice != 11 && choice != 12);     // 如果选择不是11或12，继续循环
}

// 添加新联系人的函数实现
void newItem() {
    system("cls"); // 清屏
    if (contact_count >= MAX_CONTACTS) { // 检查联系人数量是否已达上限
        print_centered("联系人数量已达上限。\n");
        return;
    }

    contact new_contact = { .name = "", .phone = "", .address = "", .group = "" };

    print_separator();
    print_centered("添加联系人");
    print_separator();
    printf("请输入姓名: ");
    fflush(stdout); // 确保提示信息立即输出
    SetEvent(hEvent); // 暂停时间线程

    scanf("%49s", new_contact.name); // 获取新联系人的姓名

    printf("请输入电话: ");
    fflush(stdout); // 确保提示信息立即输出
    SetEvent(hEvent); // 暂停时间线程

    scanf("%14s", new_contact.phone); // 获取新联系人的电话

    if (!validate_phone(new_contact.phone)) { // 验证电话格式
        printf("无效的电话号码。\n");
        return;
    }
    printf("请输入地址: ");
    fflush(stdout); // 确保提示信息立即输出
    SetEvent(hEvent); // 暂停时间线程

    scanf("%99s", new_contact.address); // 获取新联系人的地址

    printf("请输入分组: ");
    fflush(stdout); // 确保提示信息立即输出
    SetEvent(hEvent); // 暂停时间线程

    scanf("%19s", new_contact.group); // 获取新联系人的分组

    con[contact_count] = new_contact; // 将新联系人添加到联系人数组
    contact_count++; // 增加联系人计数
    print_centered("联系人已添加。\n");
}

// 查找联系人的函数实现
void find() {
    int choice; // 定义变量存储用户选择
    system("cls");
    print_separator();
    print_centered("查找联系人");
    print_separator();
    print_centered("请选择查询方式:\n");
    print_centered("1. 按姓名查询\n");
    print_centered("2. 按电话查询\n");
    print_centered("3. 显示全部\n");
    print_centered("请选择(1-3): ");
    scanf("%d", &choice); // 获取用户选择
    switch (choice) { // 根据用户选择调用相应的查找函数
    case 1:
        find_byname(); // 按姓名查找
        break;
    case 2:
        find_bytel(); // 按电话查找
        break;
    case 3:
        find_all(); // 按电话查找
        break;
    default:
        printf("无效选择。\n");
    }
}

// 按姓名查找联系人的函数实现
void find_byname() {
    char name[50]; // 定义变量存储要查找的姓名
    print_centered("请输入姓名: ");
    scanf("%49s", name); // 获取要查找的姓名
    int found = 0; // 定义变量存储是否找到联系人
    for (int i = 0; i < contact_count; i++) { // 遍历所有联系人
        if (strcmp(con[i].name, name) == 0) { // 如果找到匹配的姓名
            printf("姓名: %s 电话: %s 地址: %s 分组: %s\n", con[i].name, con[i].phone, con[i].address, con[i].group); // 打印联系人信息
            found = 1; // 标记找到联系人
        }
    }
    if (!found) { // 如果没有找到联系人
        printf("未找到联系人。\n");
    }
}

void find_all() {
    // 检查通讯录是否为空
    if (contact_count == 0) {
        // 如果为空，则打印提示信息并返回
        print_centered("通讯录中没有联系人。\n");
        return;
    }
    // 遍历通讯录中的所有联系人并打印它们的信息
    for (int i = 0; i < contact_count; i++) {
        printf("姓名: %s, 电话: %s, 地址: %s, 分组: %s\n", con[i].name, con[i].phone, con[i].address, con[i].group);
    }
}

// 按电话查找联系人的函数实现
void find_bytel() {
    char phone[15]; // 定义变量存储要查找的电话
    printf("请输入电话: ");
    scanf("%14s", phone); // 获取要查找的电话
    int found = 0; // 定义变量存储是否找到联系人
    for (int i = 0; i < contact_count; i++) { // 遍历所有联系人
        if (strcmp(con[i].phone, phone) == 0) { // 如果找到匹配的电话
            printf("姓名: %s 电话: %s 地址: %s 分组: %s\n", con[i].name, con[i].phone, con[i].address, con[i].group); // 打印联系人信息
            found = 1; // 标记找到联系人
        }
    }
    if (!found) { // 如果没有找到联系人
        printf("未找到联系人。\n");
    }
}

// 修改联系人的函数实现
void modify() {
    system("cls");
    print_separator();
    print_centered("修改联系人");
    print_separator();
    int choice; // 定义变量存储用户选择
    print_centered("请选择修改方式:\n");
    print_centered("1. 按姓名修改\n");
    print_centered("2. 按电话修改\n");
    print_centered("请选择(1-2): ");
    scanf("%d", &choice); // 获取用户选择

    switch (choice) { // 根据用户选择调用相应的修改函数
    case 1:
        modify_byname(); // 按姓名修改
        break;
    case 2:
        modify_bytel(); // 按电话修改
        break;
    default:
        print_centered("无效选择。\n");
    }
}

// 按姓名修改联系人的函数实现
void modify_byname() {
    system("cls");
    print_separator();
    print_centered("修改联系人");
    print_separator();
    char name[50]; // 定义变量存储要修改的姓名
    print_centered("请输入要修改的姓名: ");
    SetEvent(hEvent); // 暂停时间线程
    scanf("%49s", name); // 获取要修改的姓名
    int found = 0; // 定义变量存储是否找到联系人

    for (int i = 0; i < contact_count; i++) { // 遍历所有联系人
        if (strcmp(con[i].name, name) == 0) { // 如果找到匹配的姓名
            int choice; // 定义变量存储用户选择
            print_centered("请选择修改内容：\n");
            print_centered("1. 修改电话\n");
            print_centered("2. 修改地址\n");
            print_centered("3. 修改分组\n");
            print_centered("请选择(1-3): ");
            scanf("%d", &choice); // 获取用户选择

            switch (choice) { // 根据用户选择调用相应的修改函数
            case 1:
                print_centered("请输入新的电话: ");
                SetEvent(hEvent); // 暂停时间线程
                scanf("%14s", con[i].phone); // 获取新的电话

                if (!validate_phone(con[i].phone)) { // 验证电话格式
                    print_centered("无效的电话号码，请重试。\n");
                    scanf("%14s", con[i].phone); // 获取新的电话
                }

                print_centered("联系人已修改。\n");
                SetEvent(hEvent); // 暂停时间线程
                found = 1; // 标记找到联系人
                break;

            case 2:
                print_centered("请输入新的地址: ");
                SetEvent(hEvent); // 暂停时间线程
                scanf("%99s", con[i].address); // 获取新的地址
                print_centered("联系人已修改。\n");
                SetEvent(hEvent); // 暂停时间线程
                found = 1; // 标记找到联系人
                break;

            case 3:
                print_centered("请输入新的分组: ");
                SetEvent(hEvent); // 暂停时间线程
                scanf("%19s", con[i].group); // 获取新的分组
                print_centered("联系人已修改。\n");
                SetEvent(hEvent); // 暂停时间线程
                found = 1; // 标记找到联系人
                break;

            default:
                print_centered("无效选择。\n");
            }
        }
    }
    if (!found) { // 如果没有找到联系人
        printf("未找到联系人。\n");
    }
}

// 按电话修改联系人的函数实现
void modify_bytel() {
    system("cls");
    char phone[15]; // 定义变量存储要修改的电话
    print_separator();
    print_centered("修改联系人");
    print_separator();
    print_centered("请输入要修改的电话: ");
    SetEvent(hEvent); // 暂停时间线程
    scanf("%14s", phone); // 获取要修改的电话
    int found = 0; // 定义变量存储是否找到联系人

    for (int i = 0; i < contact_count; i++) { // 遍历所有联系人
        if (strcmp(con[i].phone, phone) == 0) { // 如果找到匹配的电话
            int choice; // 定义变量存储用户选择
            print_centered("请选择修改内容：\n");
            print_centered("1. 修改姓名\n");
            print_centered("2. 修改地址\n");
            print_centered("3. 修改分组\n");
            print_centered("请选择(1-3): ");
            scanf("%d", &choice); // 获取用户选择
            switch (choice) { // 根据用户选择调用相应的修改函数
            case 1:
                print_centered("请输入新的姓名: ");
                SetEvent(hEvent); // 暂停时间线程
                scanf("%49s", con[i].name); // 获取新的姓名

                print_centered("联系人已修改。\n");
                SetEvent(hEvent); // 暂停时间线程
                found = 1; // 标记找到联系人
                break;

            case 2:
                print_centered("请输入新的地址: ");
                SetEvent(hEvent); // 暂停时间线程
                scanf("%99s", con[i].address); // 获取新的地址
                print_centered("联系人已修改。\n");
                SetEvent(hEvent); // 暂停时间线程
                found = 1; // 标记找到联系人
                break;

            case 3:
                print_centered("请输入新的分组: ");
                SetEvent(hEvent); // 暂停时间线程
                scanf("%19s", con[i].group); // 获取新的分组
                print_centered("联系人已修改。\n");
                SetEvent(hEvent); // 暂停时间线程
                found = 1; // 标记找到联系人
                break;

            default:
                print_centered("无效选择。\n");
            }
        }
    }
    if (!found) { // 如果没有找到联系人
        print_centered("未找到联系人。\n");
    }
}

// 删除联系人的函数实现
void del() {
    int choice; // 定义变量存储用户选择
    system("cls");
    print_separator();
    print_centered("删除联系人");
    print_separator();
    print_centered("请选择删除方式:\n");
    print_centered("1. 删除所有联系人\n");
    print_centered("2. 删除单个联系人\n");
    print_centered("请选择(1-2): ");
    scanf("%d", &choice); // 获取用户选择

    switch (choice) { // 根据用户选择调用相应的删除函数
    case 1:
        del_all(); // 删除所有联系人
        break;
    case 2:
        del_one(); // 删除单个联系人
        break;
    default:
        printf("无效选择。\n");
    }
}

// 删除所有联系人的函数实现
void del_all() {
    printf("确定删除所有联系人吗？(y/n): ");
    char choice; // 定义变量存储用户选择
    scanf(" %c", &choice); // 获取用户选择
    if (choice == 'y' || choice == 'Y') { // 如果用户选择是
        contact_count = 0; // 将联系人计数重置为0
        printf("所有联系人已删除。\n");
    }
    else { // 如果用户选择否
        printf("操作已取消。\n");
    }
}

// 删除单个联系人的函数实现
void del_one() {
    system("cls");
    print_separator();
    print_centered("删除单个联系人");
    print_separator();
    char name[50]; // 定义变量存储要修改的姓名
    printf("请输入要删除的联系人: ");
    SetEvent(hEvent); // 暂停时间线程
    scanf("%49s", name); // 获取要修改的姓名
    int found = 0; // 定义变量存储是否找到联系人
    for (int i = 0; i < contact_count; i++) { // 遍历所有联系人
        if (strcmp(con[i].name, name) == 0) { // 如果找到匹配的姓名
            for (int j = i; j < contact_count - 1; j++) { // 将后续联系人前移
                con[j] = con[j + 1];
            }
            contact_count--; // 减少联系人计数
            printf("联系人已删除。\n");
            found = 1; // 标记找到联系人
            break; // 退出循环
        }
    }
    if (!found) { // 如果没有找到联系人
        printf("未找到联系人。\n");
    }
}

// 导出联系人的函数实现
void export_contacts() {
    system("cls");
    print_separator();
    print_centered("导出联系人");
    print_separator();
    char filename[100]; // 定义变量存储文件名
    print_centered("请输入导出文件名: ");
    scanf("%99s", filename); // 获取文件名
    FILE* file = fopen(filename, "w"); // 打开文件以写入模式
    if (file == NULL) { // 如果文件无法打开
        print_centered("无法打开文件进行导出。\n");
        return;
    }
    for (int i = 0; i < contact_count; i++) { // 遍历所有联系人
        fprintf(file, "%s %s %s %s\n", con[i].name, con[i].phone, con[i].address, con[i].group); // 将联系人信息写入文件
    }
    fclose(file); // 关闭文件
    printf("联系人已导出到%s。\n", filename);
    const char* jan_file = "D:\\Jan.txt";
    const char* feb_file = "D:\\Feb.txt";

    int jan_count = count_lines(jan_file);
    if (jan_count == -1) {
        return 1;
    }

    int feb_count = count_lines(feb_file);
    if (feb_count == -1) {
        return 1;
    }

    int diff = feb_count - jan_count;
    if (diff > 0) {
        printf("第二个月比第一个月新增%d位联系人\n", diff);
    }
    else if (diff == 0) {
        printf("第二个月和第一个月的联系人数量相同\n");
    }
    else {
        printf("第二个月比第一个月减少了%d位联系人\n", -diff);
    }

}
//D:\\J.txt

// 导入联系人的函数实现
void import_contacts() {
    char filename[100]; // 定义变量存储文件名
    printf("请输入导入文件名: ");
    scanf("%99s", filename); // 获取文件名
    FILE* file = fopen(filename, "r"); // 打开文件以读取模式
    if (file == NULL) { // 如果文件无法打开
        printf("无法打开文件进行导入。\n");
        return;
    }
    contact_count = 0; // 重置联系人计数
    while (fscanf(file, "%49s %14s %99s %19s", con[contact_count].name, con[contact_count].phone, con[contact_count].address, con[contact_count].group) != EOF) { // 读取文件中的联系人信息
        contact_count++; // 增加联系人计数
    }
    fclose(file); // 关闭文件
    printf("联系人已从%s导入。\n", filename);
}

// 备份联系人的函数实现
void backup_contacts() {
    system("cls");
    print_separator();
    print_centered("备份联系人");
    print_separator();
    FILE* file = fopen("D:\\contact_backup.txt", "w"); // 打开备份文件以写入模式
    if (file == NULL) { // 如果文件无法打开
        print_centered("无法打开备份文件。\n");
        return;
    }
    for (int i = 0; i < contact_count; i++) { // 遍历所有联系人
        fprintf(file, "%s %s %s %s\n", con[i].name, con[i].phone, con[i].address, con[i].group); // 将联系人信息写入备份文件
    }
    fclose(file); // 关闭备份文件
    print_centered("联系人已备份。\n");
}

// 恢复联系人的函数实现
void restore_contacts() {
    system("cls");
    print_separator();
    print_centered("恢复联系人");
    print_separator();
    FILE* file = fopen("D:\\contact_backup.txt", "r"); // 打开备份文件以读取模式
    if (file == NULL) { // 如果文件无法打开
        printf("无法打开备份文件。\n");
        return;
    }
    contact_count = 0; // 重置联系人计数
    while (fscanf(file, "%49s %14s %99s %19s", con[contact_count].name, con[contact_count].phone, con[contact_count].address, con[contact_count].group) != EOF) { // 读取备份文件中的联系人信息
        contact_count++; // 增加联系人计数
    }
    fclose(file); // 关闭备份文件
    printf("联系人已从备份恢复。\n");
}

// 按分组显示联系人的函数实现
void show_by_group() {
    system("cls");
    print_separator();
    print_centered("按分组显示联系人");
    print_separator();
    char group[20]; // 定义变量存储要显示的分组
    printf("请输入分组: ");
    scanf("%19s", group); // 获取要显示的分组
    int found = 0; // 定义变量存储是否找到联系人
    for (int i = 0; i < contact_count; i++) { // 遍历所有联系人
        if (strcmp(con[i].group, group) == 0) { // 如果找到匹配的分组
            printf("姓名: %s 电话: %s 地址: %s 分组: %s\n", con[i].name, con[i].phone, con[i].address, con[i].group); // 打印联系人信息
            found = 1; // 标记找到联系人
        }
    }
    if (!found) { // 如果没有找到联系人
        printf("未找到联系人。\n");
    }
}

// 显示开发者信息的函数实现
void display_developer_info() {
    system("cls");
    print_separator();
    print_centered("开发者信息");
    print_separator();
    print_centered("姓名: 郑煜腾 赵颢程\n");
    print_centered("联系方式: 23060326@hdu.edu.cn 23060327@hdu.edu.cn\n");
    print_centered("开发时间：2024年7月8日——2024年7月9日\n");
    print_centered("版本: 1.0.0");
    print_separator();
}


// 验证电话号码的函数实现
int validate_phone(const char* phone) {
    if (strlen(phone) != 11) { // 检查电话号码长度是否为11
        return 0; // 返回0表示无效
    }
    for (int i = 0; i < 11; i++) { // 检查电话号码是否全为数字
        if (phone[i] < '0' || phone[i] > '9') {
            return 0; // 返回0表示无效
        }
    }
    return 1; // 返回1表示有效
}

void print_centered(const char* str) {
    int len = strlen(str);
    int padding = (LINE_LENGTH - len) / 2;
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("%s\n", str);
}

void print_separator() {
    for (int i = 0; i < LINE_LENGTH; i++) {
        printf("=");
    }
    printf("\n");
}

// 确认返回主界面的函数实现
void confirm_return() {
    printf("按Enter键返回主菜单...");
    fflush(stdout); // 确保提示信息立即输出
    SetEvent(hEvent); // 暂停时间线程
    while (getchar() != '\n'); // 清空输入缓冲区
    getchar();
}

//void printCurrentDateTime() {
  //  while (1)
    //{
    //    char dt[20]; // 为了紧凑，设置合适的缓冲区大小
     //   time_t now = time(0);
    //    strftime(dt, sizeof(dt), "%Y-%m-%d %H:%M:%S", localtime(&now));
      //  printf("现在是北京时间：%s\r", dt); // 使用 \r 实现覆盖输出，保持在同一行更新
        //fflush(stdout); // 刷新输出缓冲区，确保立即输出

        // 等待一秒钟，以便下一次更新
        //Sleep(1000); // 1000 毫秒等于 1 秒

    //}
//}

//void printCurrentDateTime() {
  //  time_t now;
   // time(&now);
   // struct tm* local = localtime(&now);
   // char currentTime[20];
   // strftime(currentTime, sizeof(currentTime), "%Y-%m-%d %H:%M:%S", local);

   // COORD pos = { (LINE_LENGTH - strlen(currentTime)) / 2, 30 }; // 设置光标位置为控制台最后一行中央
   // SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
   // printf("现在是北京时间：%s\r", currentTime);// 使用 \r 实现覆盖输出，保持在同一行更新
   // fflush(stdout); // 刷新输出缓冲区，确保立即输出
//}
//void printCurrentDateTime() {
 //   time_t now;
 //   time(&now);
 //   struct tm* local = localtime(&now);
 //   char currentTime[20];
 //   strftime(currentTime, sizeof(currentTime), "%Y-%m-%d %H:%M:%S", local);

 //   COORD pos = { (LINE_LENGTH - strlen(currentTime)) / 2, 30 }; // 设置光标位置为控制台最后一行中央
 //   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
 //   printf("%-*s\r", LINE_LENGTH, ""); // 用空格覆盖整行
  //  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
  //  printf("现在是北京时间：%s", currentTime); // 显示新的时间
   // fflush(stdout); // 刷新输出缓冲区，确保立即输出
//}


int count_lines(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return -1;
    }

    int count = 0;
    char c;

    // Count the number of newline characters in the file
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            count++;
        }
    }

    fclose(file);
    return count;
}

