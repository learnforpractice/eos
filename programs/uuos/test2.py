from pyeoskit import eosapi
eosapi.set_nodes(['https://user-api.eoseoul.io'])
start_block = 94449890
for n in range(94449900, 94449900 + 20):
    block = None
    for i in range(3):
        try:
            block = eosapi.get_block(n)
            break
        except:
            continue
    total = 0
    for t in block.transactions:
        total += t['cpu_usage_us']
    print(total)

