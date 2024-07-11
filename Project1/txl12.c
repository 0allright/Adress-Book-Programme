#define _CRT_SECURE_NO_WARNINGS 1 // �ر�ʹ�ò���ȫ��CRT�����ľ���
#pragma warning(disable:6031)    // �رպ��Է���ֵ�ľ���
#include <stdio.h>                 // ������׼���������
#include <stdlib.h>                // ������׼��
#include <time.h>
#include <string.h>                // �����ַ�������������
#include <locale.h>                //��ȡ������������ã�locale��
#include <windows.h>              //������Windows����ϵͳ�Ͻ���ϵͳ�����
#include <time.h>
#include <conio.h>

// ������ϵ�˽ṹ��
typedef struct {                   // ����һ���ṹ�����洢��ϵ����Ϣ
	char name[50];                 // ��ϵ�˵�����
	char phone[15];                // ��ϵ�˵ĵ绰
	char address[100];             // ��ϵ�˵ĵ�ַ
	char group[20];                // ��ϵ�˵ķ���
} contact;                         // �ṹ������Ϊcontact

#define MAX_CONTACTS 1000        //�������1000λ��ϵ��
#define LINE_LENGTH 60  // ����̨�г��ȣ���չʾ��ô����

contact con[MAX_CONTACTS];         // ����һ��contact���͵����飬�洢������ϵ��
int contact_count = 0;             // ��¼��ǰ��ϵ�������ı�������ʼ��Ϊ0
HANDLE hThread;
HANDLE hEvent;
int keepRunning = 1;

//��������
void readfile();                   // ������ȡ�ļ��ĺ���
void writefile();                  // ����д���ļ��ĺ���
void mainmenu();                   // �������˵��ĺ���
void newItem();                    // ����������ϵ�˵ĺ���
void show_all();                   // ������ʾ������ϵ�˵ĺ���
void find();                       // ����������ϵ�˵ĺ���
void find_all();
void find_byname();                // ����������������ϵ�˵ĺ���
void find_bytel();                 // �������绰������ϵ�˵ĺ���
void modify();                     // �����޸���ϵ�˵ĺ���
void modify_byname();              // �����������޸���ϵ�˵ĺ���
void modify_bytel();               // �������绰�޸���ϵ�˵ĺ���
void del();                        // ����ɾ����ϵ�˵ĺ���
void del_all();                    // ����ɾ��������ϵ�˵ĺ���
void del_one();                    // ����ɾ��������ϵ�˵ĺ���
void export_contacts();            // ����������ϵ�˵ĺ���
void import_contacts();            // ����������ϵ�˵ĺ���
void backup_contacts();            // ����������ϵ�˵ĺ���
void restore_contacts();           // �����ָ���ϵ�˵ĺ���
void show_by_group();              // ������������ʾ��ϵ�˵ĺ���
int validate_phone(const char* phone); // ������֤�绰�ĺ���
int count_lines(const char* filename);
void print_centered(const char* str);	// ����һ�����������ڽ��ı����д�ӡ
void display_developer_info();	// ����һ��������������ʾ��������Ϣ
void print_separator();// ����һ�����������ڴ�ӡ�ָ���
void printCurrentDateTime();// ����һ�����������ڴ�ӡ��ǰ���ں�ʱ��
void confirm_return();// ����һ������������ȷ�Ϸ���������
DWORD WINAPI UpdateTimeThread(LPVOID lpParameter);	// ����һ������������ʱ������̵߳���ڵ�
DWORD WINAPI print_time(LPVOID lpParam);
void printCurrentDateTime();// ����һ�����������ڴ�ӡ��ǰ���ں�ʱ��

void printCurrentDateTime() {
    time_t now;
    time(&now);
    struct tm* local = localtime(&now);
    char currentTime[20];
    strftime(currentTime, sizeof(currentTime), "%Y-%m-%d %H:%M:%S", local);

    COORD pos = { (LINE_LENGTH - strlen(currentTime)) / 2, 30 }; // ���ù��λ��Ϊ����̨���һ������
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf("%-*s\r", LINE_LENGTH, ""); // �ÿո񸲸�����
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf("�����Ǳ���ʱ�䣺%s", currentTime); // ��ʾ�µ�ʱ��
    fflush(stdout); // ˢ�������������ȷ���������
}

DWORD WINAPI print_time(LPVOID lpParam) {
    while (keepRunning) {
        if (WaitForSingleObject(hEvent, 1000) == WAIT_OBJECT_0) {
            break; // �¼�����ʱ�˳�ѭ��
        }
        printCurrentDateTime();
        Sleep(1000); // ÿ�����һ��
    }
    return 0;
}


int main() {
	// ��ȡ��ǰ�Ự��Ĭ�ϴ���ҳ
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, "");
    // �����¼�����
    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hEvent == NULL) {
        printf("CreateEvent failed (%d)\n", GetLastError());
        return 1;
    }

    // ����ʱ����ʾ�߳�
    hThread = CreateThread(NULL, 0, print_time, NULL, 0, NULL);
    if (hThread == NULL) {
        printf("CreateThread failed (%d)\n", GetLastError());
        CloseHandle(hEvent);
        return 1;
    }


	readfile();                    // ��ȡ�ļ���������ϵ������
	mainmenu();                    // ��ʾ���˵�
//	while (1) {
		// ��ȡ�û�����
	//	printf("��ѡ����(1-12): ");
		//scanf("%d", &choice);
		// ����û�ѡ���˳�
//		if (choice == 12) {
		// �����¼���֪ͨʱ������߳�ֹͣ����
//			SetEvent(hEvent);
			// �ȴ�ʱ������߳̽���
	//		WaitForSingleObject(hThread, INFINITE);
			// �ر��߳̾��
		//	CloseHandle(hThread);
			// ������Դ
			//CloseHandle(hMutex);
			//CloseHandle(hEvent);
			//break;
	//	}
		// �����û�����ִ����Ӧ�Ĳ���
		// ...
//	}
     // �˳�����ǰ������Դ
    keepRunning = 0;
    SetEvent(hEvent);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);		// �ر��߳̾��
    CloseHandle(hEvent);		// ������Դ

	return 0;
}


//��������x2
DWORD WINAPI UpdateTimeThread(LPVOID lpParameter) {// ����һ������������ʱ������̵߳���ڵ�
//	char current_time[20];  // ����һ���ַ����飬���ڴ洢��ʽ�������ں�ʱ��
//	HANDLE hEvent = (HANDLE)lpParameter;  // ��ȡ���ݸ��̵߳Ĳ��������¼����
//	while (1) {
//		time_t now = time(0);		// ��ȡ��ǰʱ��
//		strftime(current_time, sizeof(current_time), "%Y-%m-%d %H:%M:%S", localtime(&now));
//		printf("�����Ǳ���ʱ�䣺%s\r", current_time);		// �����ǰʱ��
//		fflush(stdout);
//		DWORD dwWaitResult = WaitForSingleObject(hEvent, 1000);		// �ȴ�һ���ӻ��¼�������
//		if (dwWaitResult == WAIT_OBJECT_0) {// ����¼������ã��˳��߳�
//			break;
//		}
	// �����ʱ����������ʱ��
    while (keepRunning) {
        if (WaitForSingleObject(hEvent, 1000) == WAIT_OBJECT_0) {
            break; // �¼�����ʱ�˳�ѭ��
        }
        printCurrentDateTime();
    }
    return 0;
}//����ʾ��
//DWORD WINAPI UpdateTimeThread(LPVOID lpParameter) {
    // ����һ���ַ����飬���ڴ洢��ʽ�������ں�ʱ��
   // char current_time[20];
    // ��ȡ���ݸ��̵߳Ĳ��������¼����
   // HANDLE hEvent = (HANDLE)lpParameter;
    // ��������ѭ����ֱ���յ�ֹͣ�ź�
    //while (1) {
        // ��ȡ��ǰʱ��
      //  time_t now = time(0);
        // ����ǰʱ���ʽ��Ϊ�ַ���
        //strftime(current_time, sizeof(current_time), "%Y-%m-%d %H:%M:%S", localtime(&now));
        // ����������ʱ���ַ���
        // ע�⣺����û��ʹ�û���������ΪprintCurrentDateTime������û�й�����Դ
        // �����ǰʱ��
        //printf("�����Ǳ���ʱ�䣺%s\r", current_time);
        // ˢ�������������ȷ���������
        //fflush(stdout);
        // �ȴ�һ���ӻ��¼�������
        //DWORD dwWaitResult = WaitForSingleObject(hEvent, 1000);
        // ����¼������ã��˳��߳�
        //if (dwWaitResult == WAIT_OBJECT_0) {
          //  break;
        //}
        // �����ʱ����������ʱ��
    //}
    //return 0;
//}

void readfile() {                  // ��ȡ�ļ���ʵ��
    fflush(stdout); // ȷ����ʾ��Ϣ�������
    SetEvent(hEvent); // ��ͣʱ���߳�

	FILE* file = fopen("D:\\contact.txt", "r");  // ��contact.txt�ļ��Զ�ȡģʽ
	if (file == NULL) {            // ����ļ�������
		printf("�ļ�contact.txt�����ڣ��Ƿ񴴽����ļ��� (y/n): ");  // ��ʾ�û��ļ������ڣ��Ƿ񴴽����ļ�
        fflush(stdout); // ȷ����ʾ��Ϣ�������
        SetEvent(hEvent); // ��ͣʱ���߳�

        char choice;               // ����һ���������洢�û���ѡ��

		scanf(" %c", &choice);     // ��ȡ�û���ѡ��
        fflush(stdout); // ȷ����ʾ��Ϣ�������
        SetEvent(hEvent); // ��ͣʱ���߳�

		if (choice == 'y' || choice == 'Y') {  // ����û�ѡ����
			file = fopen("D:\\contact.txt", "w");  // �������ļ�
			if (file == NULL) {    // ����ļ�����ʧ��

				printf("�����ļ�ʧ�ܡ�\n");  // ��ʾ����ʧ��
                fflush(stdout); // ȷ����ʾ��Ϣ�������
                SetEvent(hEvent); // ��ͣʱ���߳�
				exit(1);           // �˳����򣬷���1
			}
			fclose(file);          // �ر��ļ�
		}
		else {                     // ����û�ѡ���
			exit(0);               // �˳����򣬷���0
		}
	}
	else {                         // ����ļ�����
		while (fscanf(file, "%49s %14s %99s %19s", con[contact_count].name, con[contact_count].phone, con[contact_count].address, con[contact_count].group) != EOF) {  // ��ȡ�ļ��е���ϵ����Ϣ
			contact_count++;       // ������ϵ�˼���
		}
		fclose(file);              // �ر��ļ�
	}
}

void writefile() {                 // д���ļ���ʵ��
	FILE* file = fopen("D:\\contact.txt", "w");  // ��contact.txt�ļ���д��ģʽ
	if (file == NULL) {            // ����ļ��޷���
		printf("�޷����ļ�����д�롣\n");  // ��ʾ�޷����ļ�
        fflush(stdout); // ȷ����ʾ��Ϣ�������
        SetEvent(hEvent); // ��ͣʱ���߳�

		return;                    // ���أ���ִ�к�������
	}
	for (int i = 0; i < contact_count; i++) {  // ����������ϵ��
		fprintf(file, "%s %s %s %s\n", con[i].name, con[i].phone, con[i].address, con[i].group);  // ����ϵ����Ϣд���ļ�
        fflush(stdout); // ȷ����ʾ��Ϣ�������
        SetEvent(hEvent); // ��ͣʱ���߳�

	}
	fclose(file);                  // �ر��ļ�
}

void mainmenu() { // ���˵���ʵ��
	int choice;                    // ��������洢�û�ѡ��
//	print_centered("��ӭʹ��ͨѶ¼С����1.0�汾");
	do {                           // ѭ����ʾ�˵�ֱ���û�ѡ���˳�
        system("cls"); // ����
//        printCurrentDateTime();
//        print_centered("��ӭʹ��ͨѶ¼С����1.0�汾");
		print_separator();
//		print_centered("ͨѶ¼����ϵͳ");
        print_centered("��ӭʹ��ͨѶ¼С����1.0�汾");
//		printf("\n");
        print_separator();
		print_centered("1. ������ϵ��");
		print_centered("2. ��ѯ��ʾ��ϵ��");
		print_centered("3. ɾ����ϵ��");
		print_centered("4. �޸���ϵ��");
		print_centered("5. ������ϵ��");
		print_centered("6. ������ϵ��");
		print_centered("7. ������ϵ��");
		print_centered("8. �ָ���ϵ��");
		print_centered("9. ��ʾ������ϵ��");
		print_centered("10. ��ʾ��������Ϣ");
		print_centered("11. �����˳�");
		print_centered("12. �������˳�");
		print_separator();
		print_centered("��ѡ����(1-12): ");
        printCurrentDateTime();
        printf("\n");

		if (scanf("%d", &choice) != 1) {// ��ȡ�û�ѡ�񲢼�������Ƿ���Ч
			while (getchar() != '\n'); // ���������
			printf("��Ч���룬���������֡�\n");
			continue; // ����ѭ��
		}

		switch (choice) {                       // �����û�ѡ��ִ����Ӧ����
			// �����¼����
//			HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//			if (hEvent == NULL) {
				// �������
//				return 1;
//			}
			// �����߳�
//			HANDLE hThread = CreateThread(NULL, 0, UpdateTimeThread, (LPVOID)hEvent, 0, NULL);
//			if (hThread == NULL) {
				// �������
//				CloseHandle(hEvent);
//				return 1;
//		}
		case 1:                                 // ���ѡ��1
			newItem();                          // ����������ϵ�˺���
			confirm_return();                   // ȷ�Ϸ���������
			break;                              // �˳�switch
		case 2:                                 // ���ѡ��2
			find();                             // ���ò�ѯ��ϵ�˺���
			confirm_return();                   // ȷ�Ϸ���������
			break;                              // �˳�switch
		case 3:                                 // ���ѡ��3
			del();                              // ����ɾ����ϵ�˺���
			confirm_return();                   // ȷ�Ϸ���������
			break;                              // �˳�switch
		case 4:                                 // ���ѡ��4
			modify();                           // �����޸���ϵ�˺���
			confirm_return();                   // ȷ�Ϸ���������
			break;                              // �˳�switch
		case 5:                                 // ���ѡ��5
			export_contacts();                  // ���õ�����ϵ�˺���
			confirm_return();                   // ȷ�Ϸ���������
			break;                              // �˳�switch
		case 6:                                 // ���ѡ��6
			import_contacts();                  // ���õ�����ϵ�˺���
			confirm_return();                   // ȷ�Ϸ���������
			break;                              // �˳�switch
		case 7:                                 // ���ѡ��7
			backup_contacts();                  // ���ñ�����ϵ�˺���
			confirm_return();                   // ȷ�Ϸ���������
			break;                              // �˳�switch
		case 8:                                 // ���ѡ��8
			restore_contacts();                 // ���ûָ���ϵ�˺���
			confirm_return();                   // ȷ�Ϸ���������
			break;                              // �˳�switch
		case 9:                                 // ���ѡ��9
			show_by_group();                    // ���ð�������ʾ��ϵ�˺���
			confirm_return();                   // ȷ�Ϸ���������
			break;                              // �˳�switch
		case 10:                                // ���ѡ��10
			display_developer_info();           // ��ʾ��������Ϣ
			confirm_return();                   // ȷ�Ϸ���������
			break;                              // �˳�switch
		case 11:                                // ���ѡ��11
			writefile();                        // ����д���ļ�����
			exit(0);                            // �˳����򣬷���0
		case 12:
            
			// �����¼���֪ͨʱ������߳�ֹͣ����
//     		if (!SetEvent(hEvent)) {
				// ���SetEventʧ�ܣ���ӡ������Ϣ
	//			printf("SetEvent failed with error %d\n", GetLastError());
				// ����ѡ���˳�������߽�������������
	//		}
			// �ȴ�ʱ������߳̽���
//			WaitForSingleObject(hThread, INFINITE);
			// �ر��߳̾��
//			CloseHandle(hThread);
			// �ر��¼����
	//		CloseHandle(hEvent);
			// �˳�����
			exit(0);
		default:                                // ���ѡ����Ч
            print_centered("��Чѡ�����������롣\n");  // ��ʾ�û���������
		}
	} while (choice != 11 && choice != 12);     // ���ѡ����11��12������ѭ��
}

// �������ϵ�˵ĺ���ʵ��
void newItem() {
    system("cls"); // ����
    if (contact_count >= MAX_CONTACTS) { // �����ϵ�������Ƿ��Ѵ�����
        print_centered("��ϵ�������Ѵ����ޡ�\n");
        return;
    }

    contact new_contact = { .name = "", .phone = "", .address = "", .group = "" };

    print_separator();
    print_centered("�����ϵ��");
    print_separator();
    printf("����������: ");
    fflush(stdout); // ȷ����ʾ��Ϣ�������
    SetEvent(hEvent); // ��ͣʱ���߳�

    scanf("%49s", new_contact.name); // ��ȡ����ϵ�˵�����

    printf("������绰: ");
    fflush(stdout); // ȷ����ʾ��Ϣ�������
    SetEvent(hEvent); // ��ͣʱ���߳�

    scanf("%14s", new_contact.phone); // ��ȡ����ϵ�˵ĵ绰

    if (!validate_phone(new_contact.phone)) { // ��֤�绰��ʽ
        printf("��Ч�ĵ绰���롣\n");
        return;
    }
    printf("�������ַ: ");
    fflush(stdout); // ȷ����ʾ��Ϣ�������
    SetEvent(hEvent); // ��ͣʱ���߳�

    scanf("%99s", new_contact.address); // ��ȡ����ϵ�˵ĵ�ַ

    printf("���������: ");
    fflush(stdout); // ȷ����ʾ��Ϣ�������
    SetEvent(hEvent); // ��ͣʱ���߳�

    scanf("%19s", new_contact.group); // ��ȡ����ϵ�˵ķ���

    con[contact_count] = new_contact; // ������ϵ����ӵ���ϵ������
    contact_count++; // ������ϵ�˼���
    print_centered("��ϵ������ӡ�\n");
}

// ������ϵ�˵ĺ���ʵ��
void find() {
    int choice; // ��������洢�û�ѡ��
    system("cls");
    print_separator();
    print_centered("������ϵ��");
    print_separator();
    print_centered("��ѡ���ѯ��ʽ:\n");
    print_centered("1. ��������ѯ\n");
    print_centered("2. ���绰��ѯ\n");
    print_centered("3. ��ʾȫ��\n");
    print_centered("��ѡ��(1-3): ");
    scanf("%d", &choice); // ��ȡ�û�ѡ��
    switch (choice) { // �����û�ѡ�������Ӧ�Ĳ��Һ���
    case 1:
        find_byname(); // ����������
        break;
    case 2:
        find_bytel(); // ���绰����
        break;
    case 3:
        find_all(); // ���绰����
        break;
    default:
        printf("��Чѡ��\n");
    }
}

// ������������ϵ�˵ĺ���ʵ��
void find_byname() {
    char name[50]; // ��������洢Ҫ���ҵ�����
    print_centered("����������: ");
    scanf("%49s", name); // ��ȡҪ���ҵ�����
    int found = 0; // ��������洢�Ƿ��ҵ���ϵ��
    for (int i = 0; i < contact_count; i++) { // ����������ϵ��
        if (strcmp(con[i].name, name) == 0) { // ����ҵ�ƥ�������
            printf("����: %s �绰: %s ��ַ: %s ����: %s\n", con[i].name, con[i].phone, con[i].address, con[i].group); // ��ӡ��ϵ����Ϣ
            found = 1; // ����ҵ���ϵ��
        }
    }
    if (!found) { // ���û���ҵ���ϵ��
        printf("δ�ҵ���ϵ�ˡ�\n");
    }
}

void find_all() {
    // ���ͨѶ¼�Ƿ�Ϊ��
    if (contact_count == 0) {
        // ���Ϊ�գ����ӡ��ʾ��Ϣ������
        print_centered("ͨѶ¼��û����ϵ�ˡ�\n");
        return;
    }
    // ����ͨѶ¼�е�������ϵ�˲���ӡ���ǵ���Ϣ
    for (int i = 0; i < contact_count; i++) {
        printf("����: %s, �绰: %s, ��ַ: %s, ����: %s\n", con[i].name, con[i].phone, con[i].address, con[i].group);
    }
}

// ���绰������ϵ�˵ĺ���ʵ��
void find_bytel() {
    char phone[15]; // ��������洢Ҫ���ҵĵ绰
    printf("������绰: ");
    scanf("%14s", phone); // ��ȡҪ���ҵĵ绰
    int found = 0; // ��������洢�Ƿ��ҵ���ϵ��
    for (int i = 0; i < contact_count; i++) { // ����������ϵ��
        if (strcmp(con[i].phone, phone) == 0) { // ����ҵ�ƥ��ĵ绰
            printf("����: %s �绰: %s ��ַ: %s ����: %s\n", con[i].name, con[i].phone, con[i].address, con[i].group); // ��ӡ��ϵ����Ϣ
            found = 1; // ����ҵ���ϵ��
        }
    }
    if (!found) { // ���û���ҵ���ϵ��
        printf("δ�ҵ���ϵ�ˡ�\n");
    }
}

// �޸���ϵ�˵ĺ���ʵ��
void modify() {
    system("cls");
    print_separator();
    print_centered("�޸���ϵ��");
    print_separator();
    int choice; // ��������洢�û�ѡ��
    print_centered("��ѡ���޸ķ�ʽ:\n");
    print_centered("1. �������޸�\n");
    print_centered("2. ���绰�޸�\n");
    print_centered("��ѡ��(1-2): ");
    scanf("%d", &choice); // ��ȡ�û�ѡ��

    switch (choice) { // �����û�ѡ�������Ӧ���޸ĺ���
    case 1:
        modify_byname(); // �������޸�
        break;
    case 2:
        modify_bytel(); // ���绰�޸�
        break;
    default:
        print_centered("��Чѡ��\n");
    }
}

// �������޸���ϵ�˵ĺ���ʵ��
void modify_byname() {
    system("cls");
    print_separator();
    print_centered("�޸���ϵ��");
    print_separator();
    char name[50]; // ��������洢Ҫ�޸ĵ�����
    print_centered("������Ҫ�޸ĵ�����: ");
    SetEvent(hEvent); // ��ͣʱ���߳�
    scanf("%49s", name); // ��ȡҪ�޸ĵ�����
    int found = 0; // ��������洢�Ƿ��ҵ���ϵ��

    for (int i = 0; i < contact_count; i++) { // ����������ϵ��
        if (strcmp(con[i].name, name) == 0) { // ����ҵ�ƥ�������
            int choice; // ��������洢�û�ѡ��
            print_centered("��ѡ���޸����ݣ�\n");
            print_centered("1. �޸ĵ绰\n");
            print_centered("2. �޸ĵ�ַ\n");
            print_centered("3. �޸ķ���\n");
            print_centered("��ѡ��(1-3): ");
            scanf("%d", &choice); // ��ȡ�û�ѡ��

            switch (choice) { // �����û�ѡ�������Ӧ���޸ĺ���
            case 1:
                print_centered("�������µĵ绰: ");
                SetEvent(hEvent); // ��ͣʱ���߳�
                scanf("%14s", con[i].phone); // ��ȡ�µĵ绰

                if (!validate_phone(con[i].phone)) { // ��֤�绰��ʽ
                    print_centered("��Ч�ĵ绰���룬�����ԡ�\n");
                    scanf("%14s", con[i].phone); // ��ȡ�µĵ绰
                }

                print_centered("��ϵ�����޸ġ�\n");
                SetEvent(hEvent); // ��ͣʱ���߳�
                found = 1; // ����ҵ���ϵ��
                break;

            case 2:
                print_centered("�������µĵ�ַ: ");
                SetEvent(hEvent); // ��ͣʱ���߳�
                scanf("%99s", con[i].address); // ��ȡ�µĵ�ַ
                print_centered("��ϵ�����޸ġ�\n");
                SetEvent(hEvent); // ��ͣʱ���߳�
                found = 1; // ����ҵ���ϵ��
                break;

            case 3:
                print_centered("�������µķ���: ");
                SetEvent(hEvent); // ��ͣʱ���߳�
                scanf("%19s", con[i].group); // ��ȡ�µķ���
                print_centered("��ϵ�����޸ġ�\n");
                SetEvent(hEvent); // ��ͣʱ���߳�
                found = 1; // ����ҵ���ϵ��
                break;

            default:
                print_centered("��Чѡ��\n");
            }
        }
    }
    if (!found) { // ���û���ҵ���ϵ��
        printf("δ�ҵ���ϵ�ˡ�\n");
    }
}

// ���绰�޸���ϵ�˵ĺ���ʵ��
void modify_bytel() {
    system("cls");
    char phone[15]; // ��������洢Ҫ�޸ĵĵ绰
    print_separator();
    print_centered("�޸���ϵ��");
    print_separator();
    print_centered("������Ҫ�޸ĵĵ绰: ");
    SetEvent(hEvent); // ��ͣʱ���߳�
    scanf("%14s", phone); // ��ȡҪ�޸ĵĵ绰
    int found = 0; // ��������洢�Ƿ��ҵ���ϵ��

    for (int i = 0; i < contact_count; i++) { // ����������ϵ��
        if (strcmp(con[i].phone, phone) == 0) { // ����ҵ�ƥ��ĵ绰
            int choice; // ��������洢�û�ѡ��
            print_centered("��ѡ���޸����ݣ�\n");
            print_centered("1. �޸�����\n");
            print_centered("2. �޸ĵ�ַ\n");
            print_centered("3. �޸ķ���\n");
            print_centered("��ѡ��(1-3): ");
            scanf("%d", &choice); // ��ȡ�û�ѡ��
            switch (choice) { // �����û�ѡ�������Ӧ���޸ĺ���
            case 1:
                print_centered("�������µ�����: ");
                SetEvent(hEvent); // ��ͣʱ���߳�
                scanf("%49s", con[i].name); // ��ȡ�µ�����

                print_centered("��ϵ�����޸ġ�\n");
                SetEvent(hEvent); // ��ͣʱ���߳�
                found = 1; // ����ҵ���ϵ��
                break;

            case 2:
                print_centered("�������µĵ�ַ: ");
                SetEvent(hEvent); // ��ͣʱ���߳�
                scanf("%99s", con[i].address); // ��ȡ�µĵ�ַ
                print_centered("��ϵ�����޸ġ�\n");
                SetEvent(hEvent); // ��ͣʱ���߳�
                found = 1; // ����ҵ���ϵ��
                break;

            case 3:
                print_centered("�������µķ���: ");
                SetEvent(hEvent); // ��ͣʱ���߳�
                scanf("%19s", con[i].group); // ��ȡ�µķ���
                print_centered("��ϵ�����޸ġ�\n");
                SetEvent(hEvent); // ��ͣʱ���߳�
                found = 1; // ����ҵ���ϵ��
                break;

            default:
                print_centered("��Чѡ��\n");
            }
        }
    }
    if (!found) { // ���û���ҵ���ϵ��
        print_centered("δ�ҵ���ϵ�ˡ�\n");
    }
}

// ɾ����ϵ�˵ĺ���ʵ��
void del() {
    int choice; // ��������洢�û�ѡ��
    system("cls");
    print_separator();
    print_centered("ɾ����ϵ��");
    print_separator();
    print_centered("��ѡ��ɾ����ʽ:\n");
    print_centered("1. ɾ��������ϵ��\n");
    print_centered("2. ɾ��������ϵ��\n");
    print_centered("��ѡ��(1-2): ");
    scanf("%d", &choice); // ��ȡ�û�ѡ��

    switch (choice) { // �����û�ѡ�������Ӧ��ɾ������
    case 1:
        del_all(); // ɾ��������ϵ��
        break;
    case 2:
        del_one(); // ɾ��������ϵ��
        break;
    default:
        printf("��Чѡ��\n");
    }
}

// ɾ��������ϵ�˵ĺ���ʵ��
void del_all() {
    printf("ȷ��ɾ��������ϵ����(y/n): ");
    char choice; // ��������洢�û�ѡ��
    scanf(" %c", &choice); // ��ȡ�û�ѡ��
    if (choice == 'y' || choice == 'Y') { // ����û�ѡ����
        contact_count = 0; // ����ϵ�˼�������Ϊ0
        printf("������ϵ����ɾ����\n");
    }
    else { // ����û�ѡ���
        printf("������ȡ����\n");
    }
}

// ɾ��������ϵ�˵ĺ���ʵ��
void del_one() {
    system("cls");
    print_separator();
    print_centered("ɾ��������ϵ��");
    print_separator();
    char name[50]; // ��������洢Ҫ�޸ĵ�����
    printf("������Ҫɾ������ϵ��: ");
    SetEvent(hEvent); // ��ͣʱ���߳�
    scanf("%49s", name); // ��ȡҪ�޸ĵ�����
    int found = 0; // ��������洢�Ƿ��ҵ���ϵ��
    for (int i = 0; i < contact_count; i++) { // ����������ϵ��
        if (strcmp(con[i].name, name) == 0) { // ����ҵ�ƥ�������
            for (int j = i; j < contact_count - 1; j++) { // ��������ϵ��ǰ��
                con[j] = con[j + 1];
            }
            contact_count--; // ������ϵ�˼���
            printf("��ϵ����ɾ����\n");
            found = 1; // ����ҵ���ϵ��
            break; // �˳�ѭ��
        }
    }
    if (!found) { // ���û���ҵ���ϵ��
        printf("δ�ҵ���ϵ�ˡ�\n");
    }
}

// ������ϵ�˵ĺ���ʵ��
void export_contacts() {
    system("cls");
    print_separator();
    print_centered("������ϵ��");
    print_separator();
    char filename[100]; // ��������洢�ļ���
    print_centered("�����뵼���ļ���: ");
    scanf("%99s", filename); // ��ȡ�ļ���
    FILE* file = fopen(filename, "w"); // ���ļ���д��ģʽ
    if (file == NULL) { // ����ļ��޷���
        print_centered("�޷����ļ����е�����\n");
        return;
    }
    for (int i = 0; i < contact_count; i++) { // ����������ϵ��
        fprintf(file, "%s %s %s %s\n", con[i].name, con[i].phone, con[i].address, con[i].group); // ����ϵ����Ϣд���ļ�
    }
    fclose(file); // �ر��ļ�
    printf("��ϵ���ѵ�����%s��\n", filename);
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
        printf("�ڶ����±ȵ�һ��������%dλ��ϵ��\n", diff);
    }
    else if (diff == 0) {
        printf("�ڶ����º͵�һ���µ���ϵ��������ͬ\n");
    }
    else {
        printf("�ڶ����±ȵ�һ���¼�����%dλ��ϵ��\n", -diff);
    }

}
//D:\\J.txt

// ������ϵ�˵ĺ���ʵ��
void import_contacts() {
    char filename[100]; // ��������洢�ļ���
    printf("�����뵼���ļ���: ");
    scanf("%99s", filename); // ��ȡ�ļ���
    FILE* file = fopen(filename, "r"); // ���ļ��Զ�ȡģʽ
    if (file == NULL) { // ����ļ��޷���
        printf("�޷����ļ����е��롣\n");
        return;
    }
    contact_count = 0; // ������ϵ�˼���
    while (fscanf(file, "%49s %14s %99s %19s", con[contact_count].name, con[contact_count].phone, con[contact_count].address, con[contact_count].group) != EOF) { // ��ȡ�ļ��е���ϵ����Ϣ
        contact_count++; // ������ϵ�˼���
    }
    fclose(file); // �ر��ļ�
    printf("��ϵ���Ѵ�%s���롣\n", filename);
}

// ������ϵ�˵ĺ���ʵ��
void backup_contacts() {
    system("cls");
    print_separator();
    print_centered("������ϵ��");
    print_separator();
    FILE* file = fopen("D:\\contact_backup.txt", "w"); // �򿪱����ļ���д��ģʽ
    if (file == NULL) { // ����ļ��޷���
        print_centered("�޷��򿪱����ļ���\n");
        return;
    }
    for (int i = 0; i < contact_count; i++) { // ����������ϵ��
        fprintf(file, "%s %s %s %s\n", con[i].name, con[i].phone, con[i].address, con[i].group); // ����ϵ����Ϣд�뱸���ļ�
    }
    fclose(file); // �رձ����ļ�
    print_centered("��ϵ���ѱ��ݡ�\n");
}

// �ָ���ϵ�˵ĺ���ʵ��
void restore_contacts() {
    system("cls");
    print_separator();
    print_centered("�ָ���ϵ��");
    print_separator();
    FILE* file = fopen("D:\\contact_backup.txt", "r"); // �򿪱����ļ��Զ�ȡģʽ
    if (file == NULL) { // ����ļ��޷���
        printf("�޷��򿪱����ļ���\n");
        return;
    }
    contact_count = 0; // ������ϵ�˼���
    while (fscanf(file, "%49s %14s %99s %19s", con[contact_count].name, con[contact_count].phone, con[contact_count].address, con[contact_count].group) != EOF) { // ��ȡ�����ļ��е���ϵ����Ϣ
        contact_count++; // ������ϵ�˼���
    }
    fclose(file); // �رձ����ļ�
    printf("��ϵ���Ѵӱ��ݻָ���\n");
}

// ��������ʾ��ϵ�˵ĺ���ʵ��
void show_by_group() {
    system("cls");
    print_separator();
    print_centered("��������ʾ��ϵ��");
    print_separator();
    char group[20]; // ��������洢Ҫ��ʾ�ķ���
    printf("���������: ");
    scanf("%19s", group); // ��ȡҪ��ʾ�ķ���
    int found = 0; // ��������洢�Ƿ��ҵ���ϵ��
    for (int i = 0; i < contact_count; i++) { // ����������ϵ��
        if (strcmp(con[i].group, group) == 0) { // ����ҵ�ƥ��ķ���
            printf("����: %s �绰: %s ��ַ: %s ����: %s\n", con[i].name, con[i].phone, con[i].address, con[i].group); // ��ӡ��ϵ����Ϣ
            found = 1; // ����ҵ���ϵ��
        }
    }
    if (!found) { // ���û���ҵ���ϵ��
        printf("δ�ҵ���ϵ�ˡ�\n");
    }
}

// ��ʾ��������Ϣ�ĺ���ʵ��
void display_developer_info() {
    system("cls");
    print_separator();
    print_centered("��������Ϣ");
    print_separator();
    print_centered("����: ֣���� ����\n");
    print_centered("��ϵ��ʽ: 23060326@hdu.edu.cn 23060327@hdu.edu.cn\n");
    print_centered("����ʱ�䣺2024��7��8�ա���2024��7��9��\n");
    print_centered("�汾: 1.0.0");
    print_separator();
}


// ��֤�绰����ĺ���ʵ��
int validate_phone(const char* phone) {
    if (strlen(phone) != 11) { // ���绰���볤���Ƿ�Ϊ11
        return 0; // ����0��ʾ��Ч
    }
    for (int i = 0; i < 11; i++) { // ���绰�����Ƿ�ȫΪ����
        if (phone[i] < '0' || phone[i] > '9') {
            return 0; // ����0��ʾ��Ч
        }
    }
    return 1; // ����1��ʾ��Ч
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

// ȷ�Ϸ���������ĺ���ʵ��
void confirm_return() {
    printf("��Enter���������˵�...");
    fflush(stdout); // ȷ����ʾ��Ϣ�������
    SetEvent(hEvent); // ��ͣʱ���߳�
    while (getchar() != '\n'); // ������뻺����
    getchar();
}

//void printCurrentDateTime() {
  //  while (1)
    //{
    //    char dt[20]; // Ϊ�˽��գ����ú��ʵĻ�������С
     //   time_t now = time(0);
    //    strftime(dt, sizeof(dt), "%Y-%m-%d %H:%M:%S", localtime(&now));
      //  printf("�����Ǳ���ʱ�䣺%s\r", dt); // ʹ�� \r ʵ�ָ��������������ͬһ�и���
        //fflush(stdout); // ˢ�������������ȷ���������

        // �ȴ�һ���ӣ��Ա���һ�θ���
        //Sleep(1000); // 1000 ������� 1 ��

    //}
//}

//void printCurrentDateTime() {
  //  time_t now;
   // time(&now);
   // struct tm* local = localtime(&now);
   // char currentTime[20];
   // strftime(currentTime, sizeof(currentTime), "%Y-%m-%d %H:%M:%S", local);

   // COORD pos = { (LINE_LENGTH - strlen(currentTime)) / 2, 30 }; // ���ù��λ��Ϊ����̨���һ������
   // SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
   // printf("�����Ǳ���ʱ�䣺%s\r", currentTime);// ʹ�� \r ʵ�ָ��������������ͬһ�и���
   // fflush(stdout); // ˢ�������������ȷ���������
//}
//void printCurrentDateTime() {
 //   time_t now;
 //   time(&now);
 //   struct tm* local = localtime(&now);
 //   char currentTime[20];
 //   strftime(currentTime, sizeof(currentTime), "%Y-%m-%d %H:%M:%S", local);

 //   COORD pos = { (LINE_LENGTH - strlen(currentTime)) / 2, 30 }; // ���ù��λ��Ϊ����̨���һ������
 //   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
 //   printf("%-*s\r", LINE_LENGTH, ""); // �ÿո񸲸�����
  //  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
  //  printf("�����Ǳ���ʱ�䣺%s", currentTime); // ��ʾ�µ�ʱ��
   // fflush(stdout); // ˢ�������������ȷ���������
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

