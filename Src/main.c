#include "stm32f4xx_hal.h"  // Main HAL library for STM32F4 series
#include "stm32f4xx_hal_gpio.h"  // GPIO-specific HAL functions
#include "stm32f4xx_hal_rcc.h"   // Clock and RCC functions
#include "stm32f4xx_hal_pwr.h"   // Power management functions
#include "stm32f4xx.h"           // Core definitions for STM32F4
// Include STM32F4 HAL headers

#define MAX_NODES 10
#define TIMEOUT 5000  // 5-second timeout for inactive nodes

typedef struct {
    int id;
    int isActive;
    uint32_t lastActiveTime;
} Node;

Node nodes[MAX_NODES];

// Function prototypes
void SystemClock_Config(void);
void GPIO_Init(void);
void EXTI_Init(void);
void addNode(int id);
void removeNode(int id);
int findActiveNode(void);
void enterLowPowerMode(void);

// Initialize nodes
void initializeNodes() {
    for (int i = 0; i < MAX_NODES; i++) {
        nodes[i].id = i;
        nodes[i].isActive = 0;
        nodes[i].lastActiveTime = 0;
    }
}

// Add a new node
void addNode(int id) {
    if (id < MAX_NODES) {
        nodes[id].isActive = 1;
        nodes[id].lastActiveTime = HAL_GetTick();  // Timestamp when node joined
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);  // LED on
    }
}

// Remove an inactive node
void removeNode(int id) {
    if (id < MAX_NODES && nodes[id].isActive) {
        nodes[id].isActive = 0;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);  // LED off
    }
}

// Find an active node
int findActiveNode() {
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].isActive) return nodes[i].id;
    }
    return -1;
}

// Enter low-power Stop Mode
void enterLowPowerMode() {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);  // LED off
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

// Handle button interrupt (simulates new node addition)
void EXTI15_10_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);

        int newNodeID = rand() % MAX_NODES;
        addNode(newNodeID);  // Add a new node

        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  // Indicate activity with LED toggle
    }
}

int main(void) {
	HAL_Init();               // Initializes the HAL library
	SystemClock_Config();      // Configures system clocks
	GPIO_Init();               // Configures GPIO
    EXTI_Init();               // Initialize external interrupts

    initializeNodes();         // Initialize nodes

    while (1) {
        int activeNode = findActiveNode();

        if (activeNode == -1) {
            enterLowPowerMode();  // Enter low-power mode if no nodes are active
        } else {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  // Blink LED
            HAL_Delay(1000);  // Simulate communication

            // Check for inactive nodes
            for (int i = 0; i < MAX_NODES; i++) {
                if (nodes[i].isActive && (HAL_GetTick() - nodes[i].lastActiveTime > TIMEOUT)) {
                    removeNode(i);  // Remove inactive node
                }
            }
        }
    }
}

// GPIO Initialization
void GPIO_Init(void) {
    // Enable GPIOA and GPIOC clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Initialize GPIOA Pin 5 (LED)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Initialize GPIOC Pin 13 (Button)
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


// External Interrupt Initialization
void EXTI_Init(void) {
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// System Clock Configuration
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}
