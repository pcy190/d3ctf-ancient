#include <iostream>
#include <stdlib.h>
#include <cstring>
#include "obfuscate.h"

extern "C" {
#include "arith_coding.h"
}

#include "fnv1a.h"

#define MAGIC_STRING "This is an ancient string, it represents the origin of all binary characters, isn't it. Let me see, it says 0,1,2,3,4,5,6,7..."
// flag length : 56
//#define FLAG "d3ctf{w0W_sEems_u_bRe4k_uP_tHe_H1DdeN_s7R_By_Ae1tH_c0De}"
#define EMBED_TARGET "\x54\x67\x69\x73\x20\x69\x73\x20\x61\x6e\x20\x61\x6e\x63\x69\x65\x6e\x74\x20\x73\x74\x72\x69\x6e\x67\x2c\x20\x69\x74\x20\x72\x65\x70\x72\x65\x73\x65\x6e\x74\x73\x20\x74\x68\x65\x20\x6f\x72\x69\x67\x69\x6e\x20\x6f\x66\x20\x61\x6c\x6c\x20\x62\x69\x6e\x61\x72\x79\x20\x63\x68\x61\x72\x61\x63\x74\x65\x72\x73\x2c\x20\x69\x73\x6e\x27\x74\x20\x69\x74\x2e\x20\x4c\x65\x74\x20\x6d\x65\x20\x73\x65\x65\x2c\x20\x69\x74\x20\x73\x61\x79\x73\x20\x30\x2c\x31\x2c\x32\x2c\x33\x2c\x34\x2c\x35\x2c\x36\x2c\x37\x2e\x2e\x2e\x67\xf3\xa3\xca\x23\x58\xa3\xd1\xf8\xc1\x96\xe3\xd7\x85\x85\xfe\xbe\x7b\xd2\x82\x59\xf4\xd8\xf0\x5f\xf5\xe2\x55\xe5\x2c\x14\xdc\xd6\xf4\x60\xf9\x89\x84\x0c\x70\x50\xb8\xf5\xde\x7f\xff\x5a\xc8\x8d\x61\xf0\x02\x00\x00\x00\x00\x00\x00"
//unsigned char magic_dis_key[] = MAGIC_STRING;
//unsigned char input_flag[56] = {0};

//unsigned char input_flag[] = FLAG;
//unsigned char target[] = EMBED_TARGET;
string target(AY_OBFUSCATE_KEY(EMBED_TARGET, 23));
string magic_string_data(AY_OBFUSCATE_KEY(MAGIC_STRING, 23));
//const int local_size = sizeof(magic_dis_key) + sizeof(input_flag); // 126 + 1 + 56 + 1 =184
const int local_size = 184; // 126 + 1 + 56 + 1 =184
unsigned char input[local_size] = {0};
//unsigned char input[local_size] = MAGIC_STRING FLAG;


//void writeup(){
//    unsigned char *decomp = static_cast<unsigned char *>(malloc(sizeof(unsigned char) * local_size * 2));
//    cout<<endl;
//    ac_state_t encoder_state;
//    init_state(&encoder_state, 16);
//    const int update_range = strlen(MAGIC_STRING), range_clear = 0;
//    reset_uniform_probability(&encoder_state);
//    decode_value_with_update(decomp, target, &encoder_state, local_size, update_range, range_clear);
//    cout << decomp << endl;
//}

unsigned char buffer[100] = {0};
ac_state_t encoder_state;

__attribute__((noinline))
void loop_for_junk(int *index) {
    int scale = 6;
    unsigned long long cpu_time_it = (SIZE_MAX >> 36 - scale * (*index) + 1) | 1;
    while (cpu_time_it--) {
        asm{
        push rax
        xor rax, rax
        jz random_label
        _emit 0x0f
        _emit 0x80
        _emit 0x87
        add rsp, 8
        random_label:
        pop rax
        };

        asm{
        push rcx
        call JMP_CODE_TAB1_000000_000
        _emit 0x0f
        _emit 0x85
        JMP_CODE_TAB1_000000_000:
        pop rcx

        push rax
        xor rax, rax
        jz random_label
        _emit 0x0f
        add rsp, 8
        random_label:
        pop rax
        pop rcx
        }
        *index += ((cpu_time_it % 2) ? 1 : -1);
    }
    *index += 1;
    return;
}

int main() {
//
//    writeup();
//    return 0;
//    cout << EMBED_TARGET << endl;
//    cout << strlen((char *) EMBED_TARGET) << endl;
//    cout << local_size << endl;

#ifdef ADD_JUNK
    asm{
    push rax
    xor rax, rax
    jz random_label
    _emit 0x80
    add rsp, 8
    random_label:
    pop rax
    };
#endif
    cout << AY_OBFUSCATE_KEY(MAGIC_STRING, 26) << endl;
    cout << AY_OBFUSCATE_KEY("Please input the flag:\n", 99);
    cin >> buffer;


    if (strlen((char *) buffer) != 56 || fnv1a::make_hash(buffer, 5) != 3697840355) { // d3ctf
        cout << AY_OBFUSCATE_KEY("Wrong. You fail to got it.\n", 25);;
        return 0;
    }

    for (int i = 0; i < 126; i++) {
        input[i] = magic_string_data[i];
    }
    for (int i = 0; i < 56; i++) {
        input[126 + i] = buffer[i];
    }

    char *output = static_cast<char *>(malloc(sizeof(unsigned char) * local_size * 2));

    // initializing encoding state
    init_state(&encoder_state, 16);

    reset_uniform_probability(&encoder_state);
    const int update_range = strlen(MAGIC_STRING), range_clear = 0;
    encode_value_with_update((unsigned char *) output, input, local_size, &encoder_state, update_range, range_clear);

    int continue_it = 0;
    int index = 0;
    while (target[index] && index <= 178) {
        if (!continue_it) {
            continue_it = 1;
        }
        if (target[index] != output[index]) {
            break;
        } else {
            index += continue_it;
        }

    }
    loop_for_junk(&index);
    if (index >= 178) {
        cout << AY_OBFUSCATE("Success!\n");
    } else {
        cout << AY_OBFUSCATE("Wrong.\n");
    }
}