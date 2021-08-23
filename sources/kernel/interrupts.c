
#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/log.h>
#include <syscalls/types.h>
#include "kernel/interrupts.h"
#include "kernel/task.h"

typedef linked_list(IrqBinding) IrqBindingEntry;
static IrqBindingEntry interrupts = {};
static Lock lock = {};

static IrqBindingEntry *irq_binding_get(Irq id)
{
    linked_list_loop(&interrupts, binding)
    {
        if (binding->data->irq->handle == id.handle)
        {
            return binding;
        }
    }

    return nullptr;
}

static void interrupt_binding_destroy(IrqBindingEntry *handler)
{
    linked_list_remove(&interrupts, handler);
}

static void irq_destroy(Irq *handler)
{
    auto binding = irq_binding_get(*handler);
    if (binding)
    {
        interrupt_binding_destroy(binding);
    }
}

static IrqBindingEntry *irq_alloc()
{

    if (interrupts.alloc == nullptr)
    {
        linked_list_init(&interrupts, alloc_global());
    }

    IrqBindingEntry *res = linked_list_insert(&interrupts, (IrqBinding){});

    return res;
}

static BrResult send_irq_handled(IrqBinding *self)
{
    BrMsg msg = self->message;
    msg.from = BR_TASK_IRQ;
    BrTask to = self->target_task;
    Task *task CLEANUP(object_cleanup) = nullptr;

    Envelope envelope CLEANUP(envelope_cleanup) = {};

    envelope_send_without_object(&envelope, &msg);

    task = (Task *)global_lookup(to, BR_OBJECT_TASK);

    if (!task)
    {
        log_unlock("error: can't send interrupt message (invalid task)");
        return BR_BAD_HANDLE;
    }

    return channel_send(task->channel, &envelope, 0, BR_IPC_SEND);
}

BrResult irq_bindings_destroy_task(const Task *target_task)
{
    linked_list_loop(&interrupts, binding)
    {
        if (binding->data->target_task == target_task->handle)
        {
            interrupt_binding_destroy(binding);
        }
    }

    return BR_SUCCESS;
}

BrResult irq_handler_bind(BrTask task, BrIrqFlags flags, Irq *irq, BrMsg *message)
{
    LOCK_RETAINER(&lock);
    log(" binding interrupt {}", irq->irq);

    if (message->flags & BR_MSG_ANY_HND)
    {
        log(" can't bind interrupts with a message containing an object");
        return BR_BAD_ARGUMENTS;
    }

    auto binding = irq_binding_get(*irq);

    if (binding == nullptr)
    {
        binding = irq_alloc();
    }

    irq_handler_ref(irq);

    binding->data->target_task = task;
    binding->data->flags = flags;
    binding->data->message = *message;
    binding->data->ack = true;
    binding->data->irq = irq;

    return BR_SUCCESS;
}

BrResult irq_handler_unbind(BrIrqFlags flags, Irq *irq)
{
    LOCK_RETAINER(&lock);
    log(" unbinding interrupt {}", irq->irq);

    auto binding = irq_binding_get(*irq);
    if (binding == nullptr)
    {
        log(" already unbinded interrupt {}", irq->irq);
        return BR_BAD_HANDLE;
    }
    binding->data->flags = flags;

    interrupt_binding_destroy(binding);
    irq_handler_deref(irq);
    return BR_SUCCESS;
}

BrResult irq_handler_ack(Irq *irq)
{
    LOCK_RETAINER(&lock);
    auto binding = irq_binding_get(*irq);

    if (binding == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    binding->data->ack = true;
    return BR_SUCCESS;
}

void irq_handler_update(BrIrq interrupt)
{
    LOCK_RETAINER(&lock);

    linked_list_loop(&interrupts, binding)
    {
        if (binding->data->irq->irq == interrupt && binding->data->ack)
        {
            send_irq_handled(binding->data);

            binding->data->ack = false;

            if (binding->data->flags & BR_IRQ_BIND_ONCE)
            {
                irq_handler_unbind(binding->data->flags, binding->data->irq);
            }
        }
    }
}

Irq *irq_create(BrIrqId id)
{
    auto intt = alloc_make(alloc_global(), Irq);
    intt->irq = id;
    object_init(base$(intt), BR_OBJECT_IRQ, (ObjectDtor *)irq_destroy);
    return intt;
}

void irq_handler_ref(Irq *self)
{
    object_ref(base$(self));
}

void irq_handler_deref(Irq *self)
{
    object_deref(base$(self));
}