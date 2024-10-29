#include <stdio.h>   
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>

#define MAX_STATES 100        
#define MAX_NEIGHBORS 100     
#define MAX_LENGTH_STATE_NAME 20  
#define INF 999999            
#define NO_SELECTION -1       
#define NO_COLOR 0            
#define RED 1                 
#define BLUE 2                
#define YELLOW 3              
#define NUM_COLORS 3          


typedef struct {
    char names[MAX_STATES][MAX_LENGTH_STATE_NAME];
    int colors[MAX_STATES][3];  
    int num_available_colors[MAX_STATES]; 
} State;


typedef struct {
    char neighbors[MAX_STATES][MAX_NEIGHBORS][MAX_LENGTH_STATE_NAME];
    int num_neighbors[MAX_STATES];                  
    int num_states;                                 
    int colors[MAX_STATES];                         
    State state;                                    
} MapColoring;

// Hàm trả về tên của màu sắc tương ứng với mã màu (1, 2, 3) = (RED, BLUE, YELLOW)
const char* getColor(int color_code) {
    if (color_code == RED) return "RED";    
    if (color_code == BLUE) return "BLUE";   
    if (color_code == YELLOW) return "YELLOW"; 
    return "NO COLOR";          
}

// Hàm khởi tạo bản đồ với số bang cho trước và thiết lập số hàng xóm và màu ban đầu cho từng bang
void initMap(MapColoring* map, int total_states) {
    map->num_states = total_states;  // Lưu lại số bang
    for (int state_index = 0; state_index < total_states; state_index++) {
        map->num_neighbors[state_index] = 0; // Ban đầu chưa có hàng xóm nào
        map->colors[state_index] = NO_COLOR;  // Ban đầu chưa có màu

        // Khởi tạo miền giá trị (màu RED, BLUE, YELLOW)
        map->state.colors[state_index][0] = RED;
        map->state.colors[state_index][1] = BLUE;
        map->state.colors[state_index][2] = YELLOW;
        map->state.num_available_colors[state_index] = NUM_COLORS;  // Ban đầu có 3 màu khả thi
    }
}

// Hàm thêm một hàng xóm cho một bang, tạo mối quan hệ láng giềng
void addNeighbor(MapColoring* map, int state_index, const char* neighbor_name) {
    int neighbor_position = map->num_neighbors[state_index]; 
    strcpy(map->neighbors[state_index][neighbor_position], neighbor_name);  // Thêm tên hàng xóm vào cuối mảng neighbors
    map->num_neighbors[state_index]++;            // Tăng số lượng hàng xóm lên
}

// Hàm in ra màu của tất cả các bang sau khi đã tô màu xong
void printMap(MapColoring map) {
    int total_states = map.num_states;  
    for (int state_index = 0; state_index < total_states; state_index++) {
        char state_name[20];
        char color[7];
        strcpy(state_name, map.state.names[state_index]);
        strcpy(color, getColor(map.colors[state_index]));

        printf("State %s: Color %s\n", state_name, color); 
    }
}

// Hàm kiểm tra xem một màu có hợp lệ để tô cho một bang không
bool isValidColor(MapColoring* map, int state_index, int color_code) {
    int total_neighbors = map->num_neighbors[state_index];  

    for (int neighbor_index = 0; neighbor_index < total_neighbors; neighbor_index++) {
        char* neighbor = map->neighbors[state_index][neighbor_index]; 
        int neighbor_actual_index = -1;

        // Tìm chỉ số của bang hàng xóm
        for (int i = 0; i < map->num_states; i++) {
            if (strcmp(map->state.names[i], neighbor) == 0) {
                neighbor_actual_index = i;
                break;
            }
        }

        // Nếu hàng xóm đã được tô màu và trùng màu, trả về false
        if (neighbor_actual_index != -1 && map->colors[neighbor_actual_index] == color_code) {
            return false;  
        }
    }

    return true;  
}

// Hàm sửa miền giá trị của một bang
bool revise(MapColoring* map, int state_index, int neighbor_index) {
    bool revised = false;

    int state_num_colors = map->state.num_available_colors[state_index];
    // Kiểm tra từng màu của bang
    for (int color_position = 0; color_position < state_num_colors; color_position++) { 
        int state_color = map->state.colors[state_index][color_position]; 
        bool valid = false; 

        // Kiểm tra xem màu có hợp lệ không với hàng xóm
        int neighbor_num_colors = map->state.num_available_colors[neighbor_index];
        for (int color = 0; color < neighbor_num_colors; color++) { 
            int neighbor_color = map->state.colors[neighbor_index][color];
            if (neighbor_color != state_color) { 
                valid = true; 
                break; 
            }
        }
    
        // Nếu không hợp lệ, xóa màu khỏi miền giá trị
        if (!valid) {
            for (int Ncolor = color_position; Ncolor < state_num_colors - 1; Ncolor++) {
                map->state.colors[state_index][Ncolor] = map->state.colors[state_index][Ncolor + 1];
            }
            state_num_colors--; // Giảm số màu khả thi
            revised = true; 
            color_position--;  // Giảm chỉ số màu để kiểm tra lại
        }
    }
        
    return revised;
}

// Hàm Arc Consistency (AC-3) kiểm tra tính nhất quán giữa các miền giá trị
bool AC3(MapColoring* map) {
    char queue[MAX_STATES * MAX_NEIGHBORS][2][MAX_LENGTH_STATE_NAME]; // Mỗi phần tử có dạng (bang, hàng xóm) với số lượng = số bang * số hàng xóm và độ dài tên bang = 20
    int front = 0, rear = 0;

    // Thêm tất cả các cặp (bang, hàng xóm) vào hàng đợi
    int num_states = map->num_states;
    for (int state_index = 0; state_index < num_states; state_index++) {
        int num_neighbors = map->num_neighbors[state_index];
        for (int neighbor_position = 0; neighbor_position < num_neighbors; neighbor_position++) {
            strcpy(queue[rear][0], map->state.names[state_index]); // Thêm tên bang vào queue
            strcpy(queue[rear][1], map->neighbors[state_index][neighbor_position]); // Thêm tên hàng xóm vào queue
            rear++;
        }
    }

    // Xử lý hàng đợi
    while (front < rear) {
        char state_name[20];
        char neighbor_name[20];
        
        // Lấy tên bang và tên hàng xóm từ queue
        strcpy(state_name, queue[front][0]);
        strcpy(neighbor_name, queue[front][1]);
        front++;

        int state_actual_index = -1;
        int neighbor_actual_index = -1;

        // Tìm chỉ số của bang
        for (int i = 0; i < map->num_states; i++) {
            if (strcmp(map->state.names[i], state_name) == 0) {
                state_actual_index = i;
            }
            if (strcmp(map->state.names[i], neighbor_name) == 0) {
                neighbor_actual_index = i;
            }
        }

        // Nếu hàng xóm tồn tại và sửa miền giá trị thì kiểm tra tiếp
        if (neighbor_actual_index != -1 && revise(map, state_actual_index, neighbor_actual_index)) {
            if (map->state.num_available_colors[state_actual_index] == 0) {
                return false;  // Không có màu khả thi
            }

            // Thêm hàng xóm khác vào hàng đợi
            for (int i = 0; i < map->num_neighbors[state_actual_index]; i++) {
                if (strcmp(map->neighbors[state_actual_index][i], neighbor_name) != 0) {
                    strcpy(queue[rear][0], map->neighbors[state_actual_index][i]);
                    strcpy(queue[rear][1], state_name);
                    rear++;
                }
            }
        }
    }

    return true;  
}

// Hàm chọn bang có ít lựa chọn màu nhất (MRV - Minimum Remaining Values)
int selectMRVState(MapColoring* map) {
    int min_num_available_colors = INF;  
    int selected_state_index = NO_SELECTION;    

    // Tìm bang với số màu khả thi ít nhất
    for (int state_index = 0; state_index < map->num_states; state_index++) {
        if (map->colors[state_index] == NO_COLOR) {
            int num_available_colors = map->state.num_available_colors[state_index];  

            if (num_available_colors < min_num_available_colors) {
                min_num_available_colors = num_available_colors;
                selected_state_index = state_index; // Cập nhật bang được chọn
            }
        }
    }

    return selected_state_index;  
}

// Hàm đệ quy để thử tô màu cho từng bang bằng phương pháp "backtracking"
bool backtrack(MapColoring* map) {
    if (!AC3(map)) {
        return false;  // Nếu không nhất quán, trả về false
    }

    int state_index = selectMRVState(map); // Chọn bang có số màu khả thi ít nhất

    if (state_index == NO_SELECTION) {
        return true;  // Nếu không còn bang nào cần tô màu, trả về true
    }

    // Thử từng màu cho bang được chọn
    for (int color_position = 0; color_position < map->state.num_available_colors[state_index]; color_position++) {
        int color_code = map->state.colors[state_index][color_position];

        // Nếu màu hợp lệ
        if (isValidColor(map, state_index, color_code)) {
            map->colors[state_index] = color_code;  // Tô màu cho bang

            // Gọi đệ quy cho bước tiếp theo
            if (backtrack(map)) {
                return true;  
            }

            // Nếu không thành công, quay lại và thử màu khác
            map->colors[state_index] = NO_COLOR;  
        }
    }

    return false;  
}

// Hàm đếm số dòng trong file, dùng để xác định số bang (state) trong bản đồ
int countLinesInFile(const char* filename) {
    FILE* file = fopen(filename, "r");  
    if (!file) {
        perror("KHONG TIM THAY TEP");  
        exit(1);
    }
    
    int total_lines = 0;      
    char buffer[256];   

    // Đếm số dòng trong file
    while (fgets(buffer, sizeof(buffer), file)) {
        total_lines++;  
    }
    
    fclose(file);  
    return total_lines;  
}

// Hàm đọc dữ liệu từ file và thêm thông tin hàng xóm cho từng bang
void readFileInput(MapColoring* map, const char* filename) {
    FILE* file = fopen(filename, "r");  
    if (!file) {
        perror("KHONG TIM THAY TEP!");  
        exit(1);
    }

    char buffer[256];  
    char state_name[20];
    char neighbor_name[20];

    // Đọc từng dòng để lấy tên bang và hàng xóm
    for (int state_index = 0; state_index < map->num_states; state_index++) {
        if (fgets(buffer, sizeof(buffer), file)) {
            sscanf(buffer, "%s", state_name);  // Đọc tên bang
            strcpy(map->state.names[state_index], state_name); // Lưu tên bang

            // Tách và thêm hàng xóm
            char* token = strtok(buffer, " ");  
            while ((token = strtok(NULL, " ")) != NULL) {
                strcpy(neighbor_name, token);  // Đọc tên hàng xóm
                addNeighbor(map, state_index, neighbor_name);  // Thêm hàng xóm bằng tên
            }
        }
    }
    
    fclose(file);  
}

// Hàm chính
int main() {
    const char* filename = "map.txt";  // Tên file chứa dữ liệu

    int num_states = countLinesInFile(filename); // Đếm số bang từ file

    MapColoring map;
    initMap(&map, num_states); // Khởi tạo bản đồ

    readFileInput(&map, filename); // Đọc dữ liệu từ file

    // Thực hiện tô màu cho bản đồ
    if (backtrack(&map)) {
        printf("Cach to:\n");  
        printMap(map); // In ra cách tô màu
    } else {
        printf("KHONG the to mau!\n");  // In ra thông báo không thể tô màu
    }

    return 0;  
}