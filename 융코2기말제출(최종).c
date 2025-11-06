#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 20 // 13장 전처리 개념 사용 (단순매크로)
#define LOGIN_RIGHT // (#ifdef, #endif개념, 55번째 줄 로그인 권한이 정의되어있어야 로그인창이 뜸)

// 6, 7주차 구조체 개념 사용
typedef struct {  // 음식의 이름과 가격 구조체
    char name[MAX_NAME];
    int price;
} MenuItem;

typedef struct {  // 전체 메뉴판 구조체
    MenuItem* items; // 가게이름, 음식들 이름, 음식들가격 (196번째 줄 읽어주시면 감사하겠습니다)
    int itemCount; // 가게이름,메뉴개수의 수
    char* category;  // #####가게목록#####
} Menu;

// 함수 원형 목록
void ProgramLogin();
void MenuShow(Menu* menu);
void CartAdd(Menu* menu, char* choose);
void CartView(Menu* menu);
void FoodOrder();
void FoodDelivery();
void DeliveryMileage();
void CartClear();
void loadMenuFile(const char* filename, Menu* menu);

char** cart; // 장바구니 (여러개의 음식을 담기위한 포인터)      << 8주차 포인터 활용 개념 사용>>
int totalprice = 0; // 음식을 담은 총 금액
int mileageCount = 0; // 내가 쌓은 마일리지 수
int couponCount = 0; // 내가 모은 쿠폰 수



// 메인함수
int main(void) {
    Menu menu;
    loadMenuFile("가게목록과음식들이름들.txt", &menu);

    int menuChoice;
    char choose[20];

    cart = (char**)malloc(100 * sizeof(char*)); // << 12주차 동적메모리 개념사용>>
    for (int i = 0; i < 100; i++) { //
        cart[i] = (char*)malloc(MAX_NAME * sizeof(char));
        strcpy(cart[i], "");  // (해당위치에 나중에 음식이름이 저장되도록 초기화 해놓았다. 음식이름들어가면 덮어써짐) 
        // << 4,5주차 문자와 문자열 개념 사용 >>
    }
    // 즉, cart는 여러개의 음식이 담길 장바구니를 나타내는 포인터이고, cart[i]는 각각의 음식을 나타내는 포인터이다
    
    #ifdef LOGIN_RIGHT // 로그인 권한이 정의되어있어야 로그인화면이 뜸
    ProgramLogin(); // 프로그램에 로그인하는 함수를 호출한다.
    #endif

    while (1) {
        printf("@@@@@배달의 단국에 오신 것을 환영합니다.@@@@@\n");
        printf("1. 음식점목록\n");
        printf("2. 나의장바구니\n");
        printf("3. 나의쿠폰\n");
        printf("4. 프로그램종료\n\n");

        printf("메뉴선택(숫자로입력): ");
        scanf("%d", &menuChoice);
        getchar(); // 입력 버퍼에 남아 있는 개행 문자를 제거한다.
        printf("\n\n");

        switch (menuChoice) { 
        case 1: // 가게목록과 음식들 이름목록이 나타나는 부분
            while (1) {
                printf("<원하는 메뉴를 선택하여 장바구니에 추가하세요!>\n");
                MenuShow(&menu); // MenuShow함수를 호출하여 프로그램에있는 모든 가게들과 각 가게의 음식이름을 보여준다.
                printf("(메뉴를 모두 고르셨다면 'ok'를 입력하여 메뉴입력을 종료합니다.)\n");
                printf("메뉴 입력: ");
                scanf("%s", choose);
                getchar();
                printf("\n");

                if (strcmp(choose, "ok") == 0)
                    break;

                CartAdd(&menu, choose); //CartAdd함수 호출하여 입력한 메뉴를 장바구니에 추가한다.
            }
            break;

        case 2: // 나의 장바구니를 확인
            printf("장바구니로 이동합니다.\n");
            CartView(&menu); // CartView함수 호출하여 나의 장바구니 목록을 확인한다.
            if (totalprice >= 5000) { // 장바구니 누적 금액이 최소 주문금액 넘겼을경우
                printf("이대로 주문하시겠습니까?(ok 입력 시 주문 창으로, 다른 키 입력 시 메뉴로): ");
                scanf("%s", choose);
                getchar();
                if (strcmp(choose, "ok") == 0) {
                    FoodOrder(); // ok입력시 FoodOrder함수 호출하여 음식 주문창으로
                    printf("\n");
                    FoodDelivery(); // 주문창에서 주문을하고나면 배달 현황을 보여주는 배달창으로 이어짐
                    CartClear(); // 배달이 완료되고 장바구니를 초기화시키는 함수
                }
            }
            else { // 최소 주문금액 맞추지 못한경우
                printf("!!!최소 주문 금액 5000원부터 주문 가능합니다.!!!\n\n");
                printf("아무 키나 입력하세요. 메뉴로 돌아갑니다: ");
                scanf("%s", choose);
                getchar(); 
                printf("\n");
            }
            break;

        case 3: // 배달로 쌓인 마일리지와 쿠폰을 확인
            DeliveryMileage(); 
            printf("아무 키나 입력 시 메뉴로 돌아갑니다: ");
            scanf("%s", choose);
            printf("\n");
            break;
        case 4: // 프로그램 종료
            printf("이용해주셔서 감사합니다!\n");
            return 1;
        default: // 잘못된 입력
            printf("잘못된 선택입니다. 다시 선택하세요.\n\n");
            break;
        }
    }

    // 메모리 해제 부분
    for (int i = 0; i < 100; i++) {
        free(cart[i]);
    }
    free(cart);
    free(menu.items);
    free(menu.category);

    return 0;
}

// 1. 메뉴판 불러오는 함수 (가게목록과음식들이름.txt)
void loadMenuFile(char* fname, Menu* menu) {
    FILE* fp = NULL;
    fp = fopen(fname, "r");  // <<< 10, 11주차 스트림과 파일 입출력 개념 사용>>>
    if (fp == NULL) {
        fprintf(stderr, "%s파일을 열 수 없습니다.\n", fname);
        exit(1);
    }

    char line[100];
    if (fgets(line, sizeof(line), fp)) { // #####가게목록##### 부분 (가게목록 = category)
        line[strcspn(line, "\n")] = 0;  //문자열에서 줄 바꿈 문자(\n)를 찾아 줄바꿈 제거하는 코드 (\0(널문자)로 대체시킴으로써)
                                        // 줄 바꿈 문자를 제거하지 않으면 #####가게목록#####\n 이런식으로 line에 저장되는 문제발생
        menu->category = (char*)malloc((strlen(line) + 1) * sizeof(char)); // (strlen(line)+1): line의 문자열 길이와 null 문자 \0의 공간을 포함한 크기이다.
        if (menu->category == NULL) {
            printf("메모리 할당 오류\n");
            exit(1);
        }
        strcpy(menu->category, line); // #####가게목록#####을 맨 윗줄에 나타내게됨
    }

    menu->itemCount = 0; // 항목 개수 (가게이름, 음식이름 포함, 196번째 줄 설명 참고해주세요)
    menu->items = (MenuItem*)malloc(100 * sizeof(MenuItem)); // 최대 100개 항목을 저장할 메모리할당

    while (fgets(line, sizeof(line), fp)) {  //<가게이름>, 음식이름 음식가격 부분 (196번째 줄 설명 참고해주시면 감사하겠습니다)
        line[strcspn(line, "\n")] = 0;
        sscanf(line, "%s %d", menu->items[menu->itemCount].name, &menu->items[menu->itemCount].price);
        //예를 들어, line이 "union버거 5000"이라면 menu->items[menu->itemCount].name에는 "union버거",
        // menu->items[menu->itemCount].price에는 5000이 저장됨.
        menu->itemCount++; // 새 항목이 추가될 때마다 itemCount 증가
    }

    fclose(fp);
}

// 2. 프로그램에 로그인하는 함수
void ProgramLogin() {
    char id[10], pw[10];
    printf("#########배달의 단국#########\n");
    printf("\n로그인하고 맛있게 먹읍시다!\n");

    printf("아이디: _________ \b\b\b\b\b\b\b\b\b\b");
    scanf("%s", id);
    getchar();

    printf("비밀번호: _________ \b\b\b\b\b\b\b\b\b\b");
    scanf("%s", pw);
    getchar();

    printf("\n\n");
    printf("로그인 성공!\n\n");
}

// 3. 음식 메뉴판을 보여주는 함수
void MenuShow(Menu* menu) { 
    printf("-----------------%s-----------------\n", menu-> category);

    for (int i = 0; i < menu->itemCount; i++) {
        if (menu->items[i].name[0] == '<') { // 가게이름을 <가게이름>으로 적었기때문에 '<'로 시작하면 가게이름으로 인식
            printf(" %-20s\n", menu->items[i].name); // 즉, 가게이름의 경우 가게이름부분만 딱 출력 (당연히 옆에 가격부분은 없음)
        }                                           
     /* 
     !배열 구조상 items[i].name에 '가게 이름'과 각 '음식들의 이름'이 '모두' 있습니다. !
     (예를들어, '<동적할당치킨>'이라는 가게이름과, 'malloc후라이드' 음식의이름이 모두 한 배열에 있는것입니다.)
     이렇게 만들고 items[i].name[0]이 '<'인 부분은 가게이름으로 구별하여
     아래 else와 달리 items[i].name만 출력하여 가게이름만 출력합니다.
     (items[i].price 가격부분 제외하고 출력. 당연, 가게이름부분은 말그대로 가게이름을 보여주는 부분이라 price부분이 없으니까))

     '가게이름'은 '음식들의 이름'과 따로 다른 배열로 구별되어야하는게 아니냐
     items[i].name에는 음식이름만 들어가게하고
     가게이름 배열은 따로 만들어야하는게 아니냐 하실수도있습니다.
     그런데 제 생각엔, 아예 가게이름 부분을 따로 배열로 만드는 거 보다
     이렇게 다 items[i].name으로 묶어서 만들고 '<'로 가게이름부분만 따로
     구별하는 것이 코드 메모리사용이나 길이면에서 더욱 낫다고 생각했습니다.
     길게 글로나마 설명드렸는데 제가 글을 잘 쓰지못해서 죄송합니다.
     내용이 꼭 잘 전달되었으면 좋겠습니다. 감사합니다.
     */


        else { // 각 음식들의 이름과 가격 출력 (가게이름이 아닌 음식들의 이름과 각 음식의 가격 출력)
            printf(" %-20s %10d원\n", menu->items[i].name, menu->items[i].price);
        }
    }
    printf("\n");
}

// 4. 고른 음식을 장바구니에 담는 함수
void CartAdd(Menu* menu, char* choose) {
    if (choose[0] == '<') {  // '<'로 시작하는 것은 가게이름이다. 따라서 장바구니에 추가불가
        printf("오류! @@@가게 이름은 장바구니에 추가할 수 없습니다.@@@\n\n");
        return;
    }

    for (int i = 0; i < menu->itemCount; i++) {
        if (strcmp(choose, menu->items[i].name) == 0) { // 내가 입력한음식이 메뉴판에 존재한다면
            printf("%s이(가) 장바구니에 추가되었습니다.\n\n", menu->items[i].name);
            strcpy(cart[i], menu->items[i].name); // 장바구니에 추가됨
            totalprice += menu->items[i].price; // 음식을 장바구니에 담으면 그 음식의 가격이 totalprice에 추가
            return;
        }
    }
    printf("선택한 메뉴가 존재하지 않습니다.\n\n");
}

// 5. 나의 장바구니 목록을 보여주는 함수
void CartView(Menu* menu) {
    printf("----------나의 장바구니 목록입니다.----------\n");
    for (int i = 0; i < menu->itemCount; i++) {
        if (strcmp(cart[i], "") != 0) {
            printf("%s %d원\n", cart[i], menu->items[i].price); // 장바구니의 담긴 음식과 음식의 가격 출력
        }
    }
    printf("\n총 금액: %d원\n", totalprice);
}

// 6. 장바구니에 담은 음식을 주문하는 함수
void FoodOrder() {
    char address[50], review[50];
    int pay;

    printf("\n----------------주문하기---------------\n\n");
    printf("총 결제할 금액: %d\n", totalprice);
    printf("\n음식을 받을 주소를 입력하세요: ");
    fgets(address, sizeof(address), stdin); 
    address[strcspn(address, "\n")] = '\0'; 

    printf("\n가게에 요청사항을 입력하세요(리뷰이벤트: 리뷰 작성시 음료수 증정 이벤트 중!): ");
    fgets(review, sizeof(review), stdin); 
    review[strcspn(review, "\n")] = '\0'; 

    while (1) {
        printf("\n결제 수단 선택(숫자를 입력)\n 1. 신용카드 2. 토스 3. 현금: ");
        scanf("%d", &pay);

        if (pay == 1) {
            printf("\n등록된 신용카드로 자동 결제가 완료되었습니다.\n");
            break;
        }
        else if (pay == 2) {
            printf("\n토스로 자동 결제가 완료되었습니다.\n");
            break;
        }
        else if (pay == 3) {
            printf("\n배달기사님께 현금 전달 부탁드립니다.(현금결제는 리뷰이벤트 적용 불가합니다.)\n");
            break;
        }
        else {
            printf("\n결제 수단을 다시 선택하세요.\n");
            continue;
        }
    }
    printf("\n------------배달주문이 완료되었습니다.------------\n\n");
}

// 7. 음식 배달 현황을 보여주는 함수
void FoodDelivery() {
    int minute = rand() % 10 + 21; // 배달 시간을 난수로 설정하였다. (21~30분)
    char choose[10];

    while (minute > 0) {
        if (minute > 15) { // 15분 이상 남았을때는 조리중으로 표시
            printf("음식을 조리중 입니다. 배달 완료까지 %d분 예상됩니다.***\n", minute);
        }
        else { // 15분 이하일때는 배달중으로 표시
            printf("라이더님이 배달을 시작했습니다. 배달 완료까지 %d분 예상됩니다.***\n", minute);
        }

        printf("r를 눌러 새로고침: "); // r을 누르면 새로고침되면서 시간이 갱신됨 (시간이 흐른다는 것을 표현하기위해)
        scanf("%s", choose);

        if (strcmp(choose, "r") == 0) {
            minute -= 5; // 한번 새로고침할때마다 5초씩 갱신됨
        }
    }

    printf("\n문 앞으로 배달이 완료되었습니다. 분실위험이 있으니 빠른 시간내에 수령하세요.\n\n");
    mileageCount++; // 배달한번 완료할때마다 마일리지 1포인트 적립
    printf("\n----마일리지 적립 완료. 누적 마일리지: %d----\n\n", mileageCount);

    if (mileageCount == 2) { // 마일리지 2점 쌓일때마다 쿠폰 증정 (2점은 너무 적은 거 같아 마일리지 5점정도에 쿠폰 하나로 하려하였으나, 
                             // 발표시간관계상 5점을 모으기가 어려워(주문 5번 반복해야됨), 2점만 모으면 쿠폰증정되도록 하였습니다.
        couponCount++;
        mileageCount -= 2; // 마일리지 2포인트 사용 후 초기화
        printf("음식 양 두배 업그레이드 쿠폰이 발급되었습니다.: %d개 (사용 시 요청사항에 기입부탁드립니다.)\n", couponCount);
    }
}

// 8. 나의 마일리지와 쿠폰 개수 현황 출력 함수
void DeliveryMileage() {
    printf("-----보유 중인 마일리지: %d-----\n", mileageCount);
    printf("-----보유 중인 쿠폰 개수: %d-----\n", couponCount);
}

// 9. 장바구니를 초기화하는 함수 (한 번 주문이 끝나면 장바구니를 비우고 새로 주문해야 되니까 초기화)
void CartClear() {
    for (int i = 0; i < 100; i++) {
        strcpy(cart[i], ""); // 장바구니를 초기화시킨다.
    }
    totalprice = 0; // 총 누적 금액을 초기화시킨다.
}



//////// 과제를 읽어주셔서 감사합니다. ////////////