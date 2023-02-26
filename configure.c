/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : configure.c
 * @brief          : The source file of the library
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 Sang Tan Truong.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "configure.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifdef PICO_DEVICES
#include "password.h"
#include "hardware/uart.h"
#define LOG_OUTPUT(X, Y) \
    uart_puts(uart0, Y); \
    uart_puts(uart0, "\r\n")
#else
#define LOG_OUTPUT(X, ...) printf(X, __VA_ARGS__);
#endif

#define CONFIGURE_MASTER_PHONE_SYNTAX_HEAD "master=\""
#define CONFIGURE_MASTER_PHONE_SYNTAX \
    "*Syntax: <master=\"xxxxxxxxxx\";PIN=\"yyyyyy\">"
#define CONFIGURE_DEFAULT_PIN "PIN=\"082308\""
#define CONFIGURE_LIST                                               \
    "1. Add phone number:<addphone=\"xxxxxxxxxxxx\";pos=y>\r\n"        \
    "2. Delete phone:<deletephone=y>\r\n"                            \
    "3. List phone:<listphone=?>\r\n"                                \
    "4. Add modbus master "                                          \
    "package:<addmodbus=\"1122334455667788\";pos=y>\r\n"             \
    "5. MQTT Topic:<mqtttopic=\"Your Topic\";pos=y>\r\n"             \
    "6. MQTT Auth:<mqttuser=\"User\";mqttpassword=\"Password\">\r\n" \
    "7. 4 - 20 mA Analog Sensor:<4-20sensor=xyz>\r\n"                \
    "8. Save settings:<save>\r\n"

#define CONFIGURE_ADD_PHONE_SYNTAX \
    "Check syntax again:<addphone=\"xxxxxxxxxxxx\";pos=y>"
#define CONFIGURE_DELETE_PHONE_SYNTAX "Check syntax again:<deltephone=y>"
#define CONFIGURE_ADD_MODBUS_SYNTAX \
    "Check syntax again:<addmodbus=\"1122334455667788\";pos=y>"
#define CONFIGURE_MQTT_TOPIC_SYNTAX \
    "Check syntax again:<mqtttopic=\"Your Topic\";pos=y>"
#define CONFIGURE_MQTT_AUTH_SYNTAX \
    "Check syntax again:<mqttuser=\"User\";mqttpassword=\"Password\">"
#define CONFIGURE_4_20_SENSOR_SYNTAX "Check syntax again:<4-20sensor=xyz>"
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
Configuration_t *pConfigure;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
void configure_init(Configuration_t *Configure) { pConfigure = Configure; }

void configure_main_process(void) {
    if (is_receive_sms()) {
        LOGUF("Receive new sms");
        if (is_master_number()) {
            process_configure_sms();
        } else {
            if (is_master_empty()) {
                LOGUF("Master Phone is empty");
                add_master_number();
            } else {
                LOGUF("It's not the master phone!!!");
            }
        }
    }
}

bool is_receive_sms(void) {
    // Fix me: Detect when receive the new sms
    return true;
}

bool is_master_number(void) {
    LOGF("Sender:%s", pConfigure->Sender);
    LOGF("Master:%s", pConfigure->identify->PhoneNumber[0]);
    if (strcmp(pConfigure->Sender, pConfigure->identify->PhoneNumber[0]) == 0)
        return true;
    return false;
}

bool is_master_empty(void) {
    LOGF("%s", pConfigure->identify->PhoneNumber[0]);
    if (pConfigure->identify->PhoneNumber[0][0] == 0) {
        return true;
    }
    return false;
}

void add_master_number(void) {
    bool is_valid = false;
    LOGUF("In Add master");
    if (strstr(pConfigure->SmsBuffer, CONFIGURE_MASTER_PHONE_SYNTAX_HEAD)) {
        LOGUF("Right head syntax");
        if (strstr(pConfigure->SmsBuffer, CONFIGURE_DEFAULT_PIN)) {
            LOGUF("Right PIN");
            char *Buffer = calloc(SMS_MESSAGE_LENGTH, sizeof(char));
            strcpy(Buffer, pConfigure->SmsBuffer);
            strtok(Buffer, "\"");
            char *tail = strtok(NULL, "\"");
            if (strlen(tail) == PHONE_LENGTH) {
                strcpy(pConfigure->identify->PhoneNumber[0], tail);
                LOGF("Add Master Phone successfull with:%s",
                     pConfigure->identify->PhoneNumber[0]);
                sprintf(Buffer, "*Configure Master Phone successfully:\"%s\"",
                        pConfigure->identify->PhoneNumber[0]);
                pConfigure->get_back(Buffer);
                is_valid = true;
            }
            free(Buffer);
        } else {
            LOGUF("Wrong PIN");
            pConfigure->get_back("*Wrong PIN!");
            is_valid = true;
        }
    }
    if (is_valid == false) {
        pConfigure->get_back(CONFIGURE_MASTER_PHONE_SYNTAX);
    }
}

void process_configure_sms(void) {
    LOGUF("In Process configure");
    if (strstr(pConfigure->SmsBuffer, "configure=?")) {
        pConfigure->get_back(CONFIGURE_LIST);
    } else if (strstr(pConfigure->SmsBuffer, "listphone=?")) {
        list_phone();
    } else if (strstr(pConfigure->SmsBuffer, "addphone=\"")) {
        if (add_phone() == false) {
            pConfigure->get_back(CONFIGURE_ADD_PHONE_SYNTAX);
        }
    } else if (strstr(pConfigure->SmsBuffer, "deletephone=") != NULL) {
        if (delete_phone() == false) {
            pConfigure->get_back(CONFIGURE_DELETE_PHONE_SYNTAX);
        }
    } else if (strstr(pConfigure->SmsBuffer, "addmodbus=") != NULL) {
        if (add_modbus() == false) {
            pConfigure->get_back(CONFIGURE_ADD_MODBUS_SYNTAX);
        }
    } else if (strstr(pConfigure->SmsBuffer, "mqtttopic=") != NULL) {
        if (add_mqtt_topic() == false) {
            pConfigure->get_back(CONFIGURE_MQTT_TOPIC_SYNTAX);
        }
    } else if (strstr(pConfigure->SmsBuffer, "mqttuser=") != NULL) {
        if (add_user_password() == false) {
            pConfigure->get_back(CONFIGURE_MQTT_AUTH_SYNTAX);
        }
    } else if (strstr(pConfigure->SmsBuffer, "4-20sensor=") != NULL) {
        if (configure_4_20_sensor() == false) {
            pConfigure->get_back(CONFIGURE_4_20_SENSOR_SYNTAX);
        }
    } else if (strstr(pConfigure->SmsBuffer, "save") != NULL) {
        LOGUF("Save configuration");
        #ifdef PICO_DEVICES
        pico_write_data((void *)pConfigure->identify, sizeof(Identifier_t));
        #endif
        pConfigure->get_back("*Save Configuration Successfully");
    } else {
        pConfigure->get_back(CONFIGURE_LIST);
    }
}

void list_phone(void) {
    LOGUF("In list phone");
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    char *BufferLine = calloc(32, sizeof(char));
    if (Buffer != NULL && BufferLine != NULL) {
        LOGUF("In list phone");
        for (int i = 1; i < PHONE_LIST; i++) {
            sprintf(BufferLine, "%d. %s\r\n", i,
                    pConfigure->identify->PhoneNumber[i]);
            strcat(Buffer, BufferLine);
        }
        pConfigure->get_back(Buffer);
    } else {
        LOGUF("Not enough space");
    }
    free(Buffer);
    free(BufferLine);
}

bool add_phone(void) {
    LOGUF("In add phone");
    bool retval = false;
    char *Phone = calloc(PHONE_LENGTH + 1, sizeof(char));
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    int iPos;
    int pos1;
    int pos2;
    int pos3;
    char *cPos;
    if (Phone == NULL || Buffer == NULL) {
        LOGUF("Not enough storage");
        return false;
    }
    cPos = strchr(pConfigure->SmsBuffer, '\"');
    pos1 = cPos - pConfigure->SmsBuffer;
    LOGF("Pos1 =%d", pos1);
    cPos = strchr(pConfigure->SmsBuffer + pos1 + 1, '\"');
    if (cPos != NULL) {
        pos2 = cPos - pConfigure->SmsBuffer;
        if (pos2 - pos1 - 1 == PHONE_LENGTH) {
            LOGF("Pos1 =%d", pos2);
            memcpy(Phone, pConfigure->SmsBuffer + pos1 + 1, pos2 - pos1 - 1);
            cPos = strstr(pConfigure->SmsBuffer, "pos=");
            if (cPos != NULL) {
                pos3 = cPos - pConfigure->SmsBuffer + 4;
                LOGF("Pos3 =%d", pos3);
                if (pConfigure->SmsBuffer[pos3] >= '0' &&
                    pConfigure->SmsBuffer[pos3] <= '9') {
                    iPos = pConfigure->SmsBuffer[pos3] - '0';
                    sprintf(Buffer, "Add phone successfully:Phone=%s,Pos=%d",
                            Phone, iPos);
                    LOGUF(Buffer);
                    pConfigure->get_back(Buffer);
                    strcpy(pConfigure->identify->PhoneNumber[iPos], Phone);
                    retval = true;
                }
            }
        }
    }
    free(Phone);
    free(Buffer);
    return retval;
}

bool delete_phone(void) {
    LOGUF("Add delete phone");
    bool retval = false;
    char *cPos = strchr(pConfigure->SmsBuffer, '=');
    int iPos = cPos - pConfigure->SmsBuffer + 1;
    char *Buffer = calloc(32, sizeof(char));
    if (Buffer == NULL) {
        LOGUF("Not enough storage");
        return false;
    }
    if (pConfigure->SmsBuffer[iPos] >= '0' &&
        pConfigure->SmsBuffer[iPos] <= '9') {
        int iPhonePos = pConfigure->SmsBuffer[iPos] - '0';
        memset(pConfigure->identify->PhoneNumber[iPhonePos], '\0',
               PHONE_LENGTH + 1);
        sprintf(Buffer, "Delete Phone %d successfully", iPhonePos);
        LOGUF(Buffer);
        pConfigure->get_back(Buffer);
        retval = true;
    } else {
        retval = false;
    }
    free(Buffer);
    return retval;
}

bool add_modbus(void) {
    LOGUF("In add modbus");
    bool retval = false;
    substr_t text;
    substr_t text2;
    text = substr(pConfigure->SmsBuffer, "=\"", "\"");
    if (text.isAvailable == true) {
        if (strlen(text.Target) != MODBUS_SIZE_PACKAGE * 2) {
            LOGUF("Modbus package length is not expected");
        } else {
            text2 = substr(pConfigure->SmsBuffer, "pos=", "\032");  // EOF
            if (text2.isAvailable == true) {
                int pos = atoi(text2.Target);
                if (pos >= 0 && pos <= MODBUS_PACKAGE - 1) {  // Restriction
                    hexa_string_to_bytes(
                        text.Target, pConfigure->identify->ModbusPacket[pos]);
                    char *Buffer = calloc(32, sizeof(char));
                    char *Buffer1 = malloc(LOG_BUFFER);
                    if (Buffer == NULL || Buffer1 == NULL) {
                        LOGUF("!!!Not enough Heap memory!!!");
                    } else {
                        char Hex[3];
                        for (int i = 0; i < MODBUS_SIZE_PACKAGE; i++) {
                            sprintf(Hex, "%X",
                                    pConfigure->identify->ModbusPacket[pos][i]);
                            strcat(Buffer, Hex);
                        }
                        sprintf(Buffer1,
                                "*Add Modbus package successfully:[pos=%d]0x%s",
                                pos, Buffer);
                        LOGUF(Buffer1);
                        pConfigure->get_back(Buffer1);
                        retval = true;
                    }
                    free(Buffer);
                    free(Buffer1);
                }
            free(text2.Target);
            }
        }
    free(text.Target);
    }
    return retval;
}

bool add_mqtt_topic(void) {
    LOGUF("In add MQTT Topic");
    bool retval = false;
    substr_t text;
    substr_t text2;
    text = substr(pConfigure->SmsBuffer, "=\"", "\"");
    if (text.isAvailable == true) {
        if (strlen(text.Target) > MQTT_TOPIC_LENGTH) {
            LOGUF("Topic length is longer than expected");
        } else {
            text2 = substr(pConfigure->SmsBuffer, "pos=", "\032");  // EOF
            if (text2.isAvailable == true) {
                int pos = atoi(text2.Target);
                if (pos >= 0 && pos <= MQTT_SIZE_TOPIC - 1) {  // Restriction
                    strcpy(pConfigure->identify->MqttTopic[pos], text.Target);
                    char *Buffer = malloc(LOG_BUFFER);
                    if (Buffer == NULL) {
                        LOGUF("!!!Not enough Heap memory!!!");
                    } else {
                        sprintf(Buffer,
                                "*Add MQTT Topic successfully:[pos=%d]%s", pos,
                                pConfigure->identify->MqttTopic[pos]);
                        LOGUF(Buffer);
                        pConfigure->get_back(Buffer);
                        retval = true;
                    }
                    free(Buffer);
                }
            free(text2.Target);
            }
        }
    free(text.Target);
    }
    return retval;
}

bool add_user_password(void) {
    LOGUF("In add MQTT User and Password");
    bool retval = false;
    substr_t text;
    substr_t text2;
    text = substr(pConfigure->SmsBuffer, "mqttuser=\"", "\"");
    text2 = substr(pConfigure->SmsBuffer, "mqttpassword=\"", "\"");
    if (text.isAvailable && text2.isAvailable) {
        if (strlen(text.Target) > MQTT_USER_LENGTH ||
            strlen(text2.Target) > MQTT_PASSWORD_LENGTH) {
            LOGUF("User name or password length is longer than expected");
        } else {
            strcpy(pConfigure->identify->MqttUser, text.Target);
            strcpy(pConfigure->identify->MqttPassword, text2.Target);
            char *Buffer = malloc(LOG_BUFFER);
            if (Buffer == NULL) {
                LOGUF("!!!Not enough Heap memory!!!");
            } else {
                sprintf(Buffer,
                        "*Add User and Password successfully:user=\"%s\" & "
                        "password=\"%s\"",
                        pConfigure->identify->MqttUser,
                        pConfigure->identify->MqttPassword);
                LOGUF(Buffer);
                pConfigure->get_back(Buffer);
                free(Buffer);
                retval = true;
            }
        }
    }
    if (text.isAvailable)
        free(text.Target);
    if (text2.isAvailable)
        free(text2.Target);
    return retval;
}

bool configure_4_20_sensor(void) {
    LOGUF("In Configure 4-20 function");
    bool retval = false;
    substr_t Text;
    Text = substr(pConfigure->SmsBuffer, "4-20sensor=", "\032");  // EOF
    if (Text.isAvailable) {
        int value = atoi(Text.Target);
        if (value >= 0 && value <= 100) {
            pConfigure->identify->FourTwentySensor = value;
            LOGF("*Configure 4-20mA Sensor Successfully:%d",
                 pConfigure->identify->FourTwentySensor);
            pConfigure->get_back("*Configure 4-20mA Sensor Successfully");
            retval = true;
        } else {
            LOGUF("Configure value out of range");
        }
        free(Text.Target);
    }
    return retval;
}

bool alert_status(float Value, float UpperThresold, float LowerThreshold) {
    LOGUF("In Alert Status");
    bool retval = false;
    if (Value < LowerThreshold || Value > UpperThresold) {
        char *Buffer = malloc(LOG_BUFFER);
        if (Buffer == NULL) {
            LOGUF("Not enough Heap memory");
        } else {
            sprintf(Buffer,
                    "Alert Alert: Over Threshold Analog Sensor->Value:%f",
                    Value);
            for (int i = 1; i < PHONE_LIST; i++) {
                if (pConfigure->identify->PhoneNumber[i][0] !=
                    '\0') {  // TODO: Verify by Regex
                    LOGF(Buffer, "\"%s\"==>%s", Buffer,
                         pConfigure->identify->PhoneNumber[i]);
                    pConfigure->send_sms(pConfigure->identify->PhoneNumber[i],
                                         Buffer);
                    retval = true;
                }
            }
            free(Buffer);
        }
    } else {
        LOGUF("Sensor in stable zone");
    }
    return retval;
}

void LOG_DETAILS(const char *File, const char *Func, int Line,
                 const char *format, ...) {
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    sprintf(Buffer, "%s:%d:%s() =>", File, Line, Func);
    LOG_OUTPUT("%s", Buffer);
    va_list vl;
    va_start(vl, format);
    vsprintf(Buffer, format, vl);
    LOG_OUTPUT("%s\r\n", Buffer);
    va_end(vl);
    free(Buffer);
}

void LOG(const char *format, ...) {
    char *Buffer = calloc(LOG_BUFFER, sizeof(char));
    va_list vl;
    va_start(vl, format);
    vsprintf(Buffer, format, vl);
    LOG_OUTPUT("%s\r\n", Buffer);
    va_end(vl);
    free(Buffer);
}

substr_t substr(char *Text, const char *HeadDelim, const char *TailDelim) {
    substr_t retval;
    retval.isAvailable = false;
    char *target = NULL;
    char *start, *end;
    if (start = strstr(Text, HeadDelim)) {
        start += strlen(HeadDelim);
        end = (*TailDelim == '\032') ? (start + strlen(Text))
                                     : strstr(start, TailDelim);
        if (end) {
            target = (char *)malloc(end - start + 1);
            if (target == NULL) {
                LOGUF("!!!Not enough Heap memory!!!");
                return retval;
            }
            memcpy(target, start, end - start);
            target[end - start] = '\0';
            retval.isAvailable = true;
            retval.Target = target;
        }
    }
    return retval;
}

bool hexa_string_to_bytes(char *Text, unsigned char *Target) {
    size_t length = strlen(Text);
    if (length % 2 != 0) return false;
    for (size_t i = 0, j = 0; i < (length / 2); i++, j += 2) {
        Target[i] = (Text[j] % 32 + 9) % 25 * 16 + (Text[j + 1] % 32 + 9) % 25;
    }
    return true;
}
