#include "nrf_mesh.h"
#include "nrf_mesh_config_examples.h"
#include "access_config.h"
#include "health_client.h"
#include "log.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_pwr_mgmt.h"

// Constants and Model Definitions
#define MODEL_ID                     0x1000
#define COMPANY_ID                   0x0059  // Nordic Semiconductor's Company ID
#define SIMPLE_MESSAGE_OPCODE        0x01
#define SIMPLE_MESSAGE_REPLY_OPCODE  0x02

// Global Variables
static access_model_handle_t m_model_handle;
static health_client_t m_health_client;

// Function Prototypes
void mesh_stack_init(void);
void health_client_init(void);
void send_reliable_message(uint16_t dst_addr, uint8_t *data, uint16_t length);
void update_routing_table(uint16_t node_addr, uint8_t status);
void recalculate_routes(void);
void power_management_init(void);
void idle_state_handle(void);

// Initialize Mesh Stack
void mesh_stack_init(void)
{
    uint32_t err_code;

    // Initialize SoftDevice handler
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure BLE stack using default settings
    ble_stack_init();

    // Initialize Mesh Stack
    nrf_mesh_init_params_t init_params = {
        .irq_priority = NRF_MESH_IRQ_PRIORITY_LOWEST,
        .lfclksrc = NRF_CLOCK_LF_SRC_XTAL,
        .p_uuid = NULL,
    };

    err_code = nrf_mesh_init(&init_params);
    APP_ERROR_CHECK(err_code);
}

// Reliable Message Status Callback
void reliable_message_status_cb(access_model_handle_t handle,
                                void *p_args,
                                access_reliable_status_t status)
{
    if (status == ACCESS_RELIABLE_TRANSFER_SUCCESS)
    {
        LOG_INFO("Message sent successfully.");
    }
    else
    {
        LOG_WARNING("Message failed to send. Retrying...");
    }
}

// Send Reliable Message
void send_reliable_message(uint16_t dst_addr, uint8_t *data, uint16_t length)
{
    access_reliable_t reliable;
    memset(&reliable, 0, sizeof(reliable));

    reliable.model_handle = m_model_handle;
    reliable.message.p_buffer = data;
    reliable.message.length = length;
    reliable.message.opcode.opcode = SIMPLE_MESSAGE_OPCODE;
    reliable.message.opcode.company_id = COMPANY_ID;
    reliable.reply_opcode.opcode = SIMPLE_MESSAGE_REPLY_OPCODE;
    reliable.reply_opcode.company_id = COMPANY_ID;
    reliable.timeout = ACCESS_RELIABLE_TIMEOUT_DEFAULT;
    reliable.status_cb = reliable_message_status_cb;

    uint32_t status = access_model_reliable_publish(&reliable);
    if (status != NRF_SUCCESS)
    {
        LOG_ERROR("Failed to send reliable message.");
    }
}

// Health Client Event Callback
void health_event_cb(const health_client_t *p_client, const health_client_evt_t *p_evt)
{
    if (p_evt->evt_type == HEALTH_CLIENT_EVT_TYPE_CURRENT_STATUS_RECEIVED)
    {
        if (p_evt->p_status->fault_array_length > 0)
        {
            LOG_WARNING("Node %u reported faults.", p_evt->p_meta->src.value);
            update_routing_table(p_evt->p_meta->src.value, 0); // Node is faulty
        }
        else
        {
            LOG_INFO("Node %u is healthy.", p_evt->p_meta->src.value);
            update_routing_table(p_evt->p_meta->src.value, 1); // Node is healthy
        }
    }
}

// Initialize Health Client
void health_client_init(void)
{
    m_health_client.settings.p_callbacks = &health_event_cb;
    m_health_client.settings.timeout = 0;
    m_health_client.settings.p_publication_metadata = NULL;

    uint32_t status = health_client_init(&m_health_client, 0);
    APP_ERROR_CHECK(status);
}

// Update Routing Table
void update_routing_table(uint16_t node_addr, uint8_t status)
{
    // Example: Update the routing table with the node's health status
    LOG_INFO("Updating routing table for node %u. Status: %s", 
             node_addr, status ? "Healthy" : "Faulty");
    recalculate_routes();
}

// Recalculate Routes
void recalculate_routes(void)
{
    LOG_INFO("Recalculating routes based on node statuses...");
    // Implement routing algorithm (e.g., AODV or DSR)
}

// Power Management Initialization
void power_management_init(void)
{
    ret_code_t err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

// Handle Idle State
void idle_state_handle(void)
{
    nrf_pwr_mgmt_run();
}

// Main Function
int main(void)
{
    // Initialize Hardware
    hardware_init();

    // Initialize Logging
    log_init();

    // Initialize Mesh Stack
    mesh_stack_init();

    // Initialize Models
    health_client_init();

    // Start Mesh Stack
    uint32_t err_code = nrf_mesh_enable();
    APP_ERROR_CHECK(err_code);

    LOG_INFO("Self-Healing Mesh Network Initialized.");

    while (true)
    {
        // Mesh Processing
        (void)nrf_mesh_process();

        // Application-Specific Processing
        application_process();

        // Enter Low Power Mode When Idle
        idle_state_handle();
    }
}
