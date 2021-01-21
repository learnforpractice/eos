def is_reach_a_consensus(messages):
    if len(messages) < 15:
        return
    bad_block_message_count = 0
    for validator, block_id, apply_time in messages:
        # 5s - 4.5s, 0.5 second bias for eliminate block apply time errors on different host
        if apply_time > 4.5s:
            bad_block_message_count += 1
    if bad_block_message_count >= 15:
        handle_bad_block()
    else:
        handle_wrong_alarm()

def on_receive_new_block(block_id):
    apply_time = get_total_block_apply_time(block_id)
    if apply_time > 5s:
        fire_block_alarm_message(current_validator, block_id, apply_time)
        save_bad_block(block_id, apply_time)

def on_receive_block_alarm_message(validator, block_id, apply_time):
    messages.append((validator, block_id, apply_time))
    if is_reach_a_consensus(messages)
        return True

    if is_block_alarm_message_fired(block_id): #already checked
        return False

    apply_time = get_total_block_apply_time(block_id)
    fire_block_alarm_message(current_validator, block_id, apply_time)
    messages.append((current_validator, block_id, apply_time))

    if is_reach_a_consensus(messages)
        return True
    return False
