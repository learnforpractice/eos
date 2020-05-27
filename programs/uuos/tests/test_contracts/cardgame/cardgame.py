import struct
import db

#game_status:
ONGOING     = 0
PLAYER_WON   = 1
PLAYER_LOST  = -1

# enum card_type
EMPTY = 0
FIRE = 1
WOOD = 2
WATER = 3
NEUTRAL = 4
VOID = 5 

#[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17] #vector<uint8_t>
default_deck = b'\x01\x02\x03\x04\x05\x06\x07\x08\t\n\x0b\x0c\r\x0e\x0f\x10\x11'


class Card:
    def __init__(self, _type, attack_point):
        self._type = _type
        self.attack_point = attack_point

card_dict = {
    0: Card(EMPTY, 0),
    1: Card(FIRE, 1),
    2: Card(FIRE, 1),
    3: Card(FIRE, 2),
    4: Card(FIRE, 2),
    5: Card(FIRE, 3),
    6: Card(WOOD, 1),
    7: Card(WOOD, 1),
    8: Card(WOOD, 2),
    9: Card(WOOD, 2),
    10: Card(WOOD, 3), 
    11: Card(WATER, 1),
    12: Card(WATER, 1),
    13: Card(WATER, 2),
    14: Card(WATER, 2),
    15: Card(WATER, 3),
    16: Card(NEUTRAL, 3), 
    17: Card(VOID, 0)
}

class Game:
    def __init__(self):
        self.life_player = 5 #int8_t
        self.life_ai = 5 #int8_t
        self.deck_player = bytearray(default_deck) #b'\x01\x02\x03\x04\x05\x06\x07\x08\t\n\x0b\x0c\r\x0e\x0f\x10\x11' #[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17] #vector<uint8_t>
        self.deck_ai = bytearray(default_deck) #b'\x01\x02\x03\x04\x05\x06\x07\x08\t\n\x0b\x0c\r\x0e\x0f\x10\x11' #[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17] #vector<uint8_t>
        self.hand_player = bytearray(b'\x00\x00\x00\x00') #[0, 0, 0, 0] #vector<uint8_t>
        self.hand_ai = bytearray(b'\x00\x00\x00\x00') #[0, 0, 0, 0] #vector<uint8_t>
        self.selected_card_player = 0 #uint8_t
        self.selected_card_ai = 0 #uint8_t
        self.life_lost_player = 0 #uint8_t
        self.life_lost_ai = 0 #uint8_t
        self.status = ONGOING #int8

    def pack_bytes(self, buffer, index, data):
        length = len(data)
        buffer[index] = length
        index+=1
        buffer[index:index+length] = data
        index += length
        return index

    @classmethod
    def unpack_bytes(cls, buffer, index):
        length = buffer[index]
        index += 1
        return index+length, buffer[index:index+length]

    def pack(self):
        data = bytearray(49+4)
        data[0] = self.life_player
        data[1] = self.life_ai
        data[2] = self.selected_card_player
        data[3] = self.selected_card_ai
        data[4] = self.life_lost_player
        data[5] = self.life_lost_ai
        data[6] = self.status

        index = 7
        index = self.pack_bytes(data, index, self.deck_player)
        index = self.pack_bytes(data, index, self.deck_ai)
        index = self.pack_bytes(data, index, self.hand_player)
        index = self.pack_bytes(data, index, self.hand_ai)

        return data[:index]

    @classmethod
    def unpack(cls, data):
        game = Game()
        game.life_player = data[0]
        game.life_ai = data[1]

        game.selected_card_player = data[2]
        game.selected_card_ai = data[3]
        game.life_lost_player = data[4]
        game.life_lost_ai = data[5]
        game.status = data[6]

        index = 7

        index, game.deck_player = cls.unpack_bytes(data, index)
        index, game.deck_ai = cls.unpack_bytes(data, index)
        index, game.hand_player = cls.unpack_bytes(data, index)
        index, game.hand_ai = cls.unpack_bytes(data, index)

        return game

class UserInfo(object):
    def __init__(self, username):
        self.username = username
        self.win_count = 0
        self.lost_count = 0
        self.game_data = Game()

    def pack(self):
        print(self.username)
        if not isinstance(self.username, int):
            username = N(self.username)
        else:
            username =self.username

        game_data = self.game_data.pack()
        return struct.pack('QHH49s', username, self.win_count, self.lost_count, game_data)

    @classmethod
    def unpack(cls, data):
        info = UserInfo('')
        info.username, info.win_count, info.lost_count, game_data = \
            struct.unpack('QHH49s', data)

        game_data = bytearray(game_data)
        game_data = Game.unpack(game_data)
        info.game_data = game_data
        return info

    @property
    def primary_key(self):
        return self.username

    def get_primary_key(self):
        return self.username

    def get_secondary_values(self):
        return ()

    def set_secondary_value(self, idx):
        pass

    @classmethod
    def get_secondary_indexes(self):
        return ()

class Seed(object):
    def __init__(self):
        self.key = 1
        self.value = 1

    def pack(self):
        return struct.pack('QI', self.key, self.value)

    @classmethod
    def unpack(cls, data):
        key, value = struct.unpack('QI', data)
        seed = Seed()
        seed.key = key
        seed.value = value
        return seed

    @property
    def primary_key(self):
        return self.key

    def get_primary_key(self):
        return self.key

    def get_secondary_values(self):
        return ()

    @classmethod
    def get_secondary_indexes(self):
        return ()

    def set_secondary_value(self, idx):
        pass

def random(n):
    return int(current_time() / 1e6) % n

# Draw one card from the deck and assign it to the hand
def draw_one_card(deck, hand):
    #Pick a random card from the deck
    deck_card_idx = random(len(deck))

    #Find the first empty slot in the hand
    first_empty_slot = -1
    for i in range(len(hand)):
        id = hand[i]
        if card_dict[id]._type == EMPTY:
            first_empty_slot = i
            break
    eosio_assert(first_empty_slot != -1, "No empty slot in the player's hand")
    hand[first_empty_slot] = deck[deck_card_idx]
  
    # Remove the card from the deck
    del deck[deck_card_idx]

# Calculate the final attack point of a card after taking the elemental bonus into account
def calculate_attack_point(card1, card2):
    result = card1.attack_point
    #Add elemental compatibility bonus of 1
    if ((card1._type == FIRE and card2._type == WOOD) or \
        (card1._type == WOOD and card2._type == WATER) or \
        (card1._type == WATER and card2._type == FIRE)):
        result+=1

    return result

# AI Best Card Win Strategy
def ai_best_card_win_strategy(ai_attack_point: int, player_attack_point: int):
#    print("Best Card Wins")
    if ai_attack_point > player_attack_point:
        return 3
    if ai_attack_point < player_attack_point:
        return -2
    return -1

# AI Minimize Loss Strategy
def ai_min_loss_strategy(ai_attack_point: int, player_attack_point: int):
#    print("Minimum Losses")
    if ai_attack_point > player_attack_point:
        return 1
    if ai_attack_point < player_attack_point:
        return -4
    return -1

# AI Points Tally Strategy
def ai_points_tally_strategy(ai_attack_point:int, player_attack_point:int):
#    print("Points Tally")
    return ai_attack_point - player_attack_point

# AI Loss Prevention Strategy
def ai_loss_prevention_strategy(life_ai:int, ai_attack_point:int, player_attack_point:int):
    print("Loss Prevention")
    if life_ai + ai_attack_point - player_attack_point > 0:
        return 1
    return 0

# Calculate the score for the current ai card given the  strategy and the player hand cards
def calculate_ai_card_score(strategy_idx:int, life_ai:int, ai_card:Card, hand_player:list):
    card_score = 0
    for i in range(len(hand_player)):
        player_card_id = hand_player[i]
        player_card = card_dict[player_card_id]

        ai_attack_point = calculate_attack_point(ai_card, player_card)
        player_attack_point = calculate_attack_point(player_card, ai_card)

        #Accumulate the card score based on the given strategy
        if strategy_idx == 0:
            card_score += ai_best_card_win_strategy(ai_attack_point, player_attack_point)
        elif strategy_idx == 1:
            card_score += ai_min_loss_strategy(ai_attack_point, player_attack_point)
        elif strategy_idx == 2:
            card_score += ai_points_tally_strategy(ai_attack_point, player_attack_point)
        else:
            card_score += ai_loss_prevention_strategy(life_ai, ai_attack_point, player_attack_point)
    return card_score

# Chose a card from the AI's hand based on the current game data
def ai_choose_card(game_data:Game):
  # The 4th strategy is only chosen in the dire situation
    available_strategies = 4
    if game_data.life_ai > 2:
        available_strategies-=1
    strategy_idx = random(available_strategies)
 
    # Calculate the score of each card in the AI hand 
    chosen_card_idx = -1
    chosen_card_score = 0 #std::numeric_limits<int>::min()

    for i in range(len(game_data.hand_ai)):
        ai_card_id = game_data.hand_ai[i]
        ai_card = card_dict[ai_card_id]
        # Ignore empty slot in the hand
        if ai_card._type == EMPTY:
            continue

        # Calculate the score for this AI card relative to the player's hand cards
        card_score = calculate_ai_card_score(strategy_idx, game_data.life_ai, ai_card, game_data.hand_player)

        # Keep track of the card that has the highest score
        if card_score > chosen_card_score:
            chosen_card_score = card_score
            chosen_card_idx = i
    return chosen_card_idx

#  Resolve selected cards and update the damage dealt
def resolve_selected_cards(game_data:Game):
    player_card = card_dict[game_data.selected_card_player]
    ai_card = card_dict[game_data.selected_card_ai]

    #  For type VOID, we will skip any damage calculation
    if player_card._type == VOID or ai_card._type == VOID:
        return

    player_attack_point = calculate_attack_point(player_card, ai_card)
    ai_attack_point =  calculate_attack_point(ai_card, player_card)

    #  Damage calculation
    if player_attack_point > ai_attack_point:
        #  Deal damage to the AI if the AI card's attack point is higher
        diff = player_attack_point - ai_attack_point
        game_data.life_lost_ai = diff
        game_data.life_ai -= diff
    elif ai_attack_point > player_attack_point:
    #  Deal damage to the player if the player card's attack point is higher
        diff = ai_attack_point - player_attack_point
        game_data.life_lost_player = diff
        game_data.life_player -= diff

# Check the current game board and update the game status accordingly
def update_game_status(user:UserInfo):
    game_data = user.game_data
    if game_data.life_ai <= 0:
        # Check the AI's HP
        game_data.status = PLAYER_WON
    elif game_data.life_player <= 0:
        # Check the player's HP
        game_data.status = PLAYER_LOST
    else:
        # Neither player has their HP reduced to 0
        # Check whether the game has finished (i.e., no more cards in both hands)
        player_finished = True
        for i in game_data.hand_player:
            if not card_dict[i] == EMPTY:
                player_finished = False
                break

        ai_finished = True
        for i in game_data.hand_ai:
            if not card_dict[i] == EMPTY:
                ai_finished = False
                break

        # If one of them has run out of card, the other must have run out of card too
        if player_finished or ai_finished:
            if game_data.life_player > game_data.life_ai:
                game_data.status = PLAYER_WON
            else:
                game_data.status = PLAYER_LOST

  # Update the lost/ win count accordingly
    if game_data.status == PLAYER_WON:
        user.win_count+=1
    elif game_data.status == PLAYER_LOST:
        user.lost_count+=1

def login(username):
    # Ensure this action is authorized by the player
    require_auth(username)
  
    # Create a record in the table if the player doesn't exist in our app yet
    user_iterator = users.find(username)
    if user_iterator < 0:
        info = UserInfo(username)
        info.payer = username
        users.store(info)
    else:
        info = users.get(user_iterator)
        # print(info.game_data.deck_player)

def startgame(username):
    # Ensure this action is authorized by the player
    require_auth(username)
    itr = users.find(username)
    assert itr >= 0, "User doesn't exist"
    user = users.get(itr)

    # Create a new game
    game_data = Game()
    # Draw 4 cards each for the player and the AI
    for i in range(4):
        draw_one_card(game_data.deck_player, game_data.hand_player)
        draw_one_card(game_data.deck_ai, game_data.hand_ai)
    # Assign the newly created game to the player
    user.game_data = game_data
    user.payer = username
    users.store(user)

def endgame(username):
    # Ensure this action is authorized by the player
    require_auth(username)

    # Get the user and reset the game
    itr = users.find(username)
    assert itr >= 0, "User doesn't exist"
    user = users.get(itr)

    user.game_data = Game()
    user.payer = username
    users.store(user)

def playcard(username, player_card_idx):
    # Ensure this action is authorized by the player
    require_auth(username)

    # Checks that selected card is valid
    assert player_card_idx < 4, "playcard: Invalid hand index"

    itr = users.find(username)
    assert itr >= 0, "User doesn't exist"
    user = users.get(itr)

    # Verify game status is suitable for the player to play a card
    assert user.game_data.status == ONGOING, \
               "playcard: This game has ended. Please start a new one"

    assert user.game_data.selected_card_player == 0, \
               "playcard: The player has played his card this turn!"

    game_data = user.game_data

    # Assign the selected card from the player's hand
    game_data.selected_card_player = game_data.hand_player[player_card_idx]
    game_data.hand_player[player_card_idx] = 0

    # AI picks a card
    ai_card_idx = ai_choose_card(game_data)
    game_data.selected_card_ai = game_data.hand_ai[ai_card_idx]
    game_data.hand_ai[ai_card_idx] = 0

    resolve_selected_cards(game_data)
    update_game_status(user)

    user.payer = username
    users.store(user)

def nextround(username):
    # Ensure this action is authorized by the player
    require_auth(username)

    itr = users.find(username)
    assert itr >= 0, "User doesn't exist"
    user = users.get(itr)

    # Verify game status
    assert user.game_data.status == ONGOING, \
                "nextround: This game has ended. Please start a new one."
    assert user.game_data.selected_card_player != 0 and user.game_data.selected_card_ai != 0, \
                "nextround: Please play a card first."

    game_data = user.game_data

    # Reset selected card and damage dealt
    game_data.selected_card_player = 0
    game_data.selected_card_ai = 0
    game_data.life_lost_player = 0
    game_data.life_lost_ai = 0

    # Draw card for the player and the AI
    if game_data.deck_player.size() > 0:
        draw_one_card(game_data.deck_player, game_data.hand_player)
    if game_data.deck_ai.size() > 0:
        draw_one_card(game_data.deck_ai, game_data.hand_ai)

    user.payer = username
    users.store(user)


code = N('helloworld11')
scope = N('scopee')
table = N('users')
users= db.MultiIndex(code, scope, table, UserInfo)

code = N('helloworld11')
scope = N('scopee')
table = N('seed')
seeds = db.MultiIndex(code, scope, table, UserInfo)

def apply(receiver, code, action):
    if action == N('login'):
        username = read_action_data()
        username, = struct.unpack('Q', username)
        # print(username)
        login(username)
        # import _db
        # itr = _db.find_i64('helloworld11', 'scopee', 'users', 'helloworld11')
        # print(itr, _db.get_i64(itr))
    elif action == N('startgame'):
        username = read_action_data()
        username, = struct.unpack('Q', username)
        startgame(username)
    elif action == N('endgame'):
        username = read_action_data()
        username, = struct.unpack('Q', username)
        endgame(username)
    elif action == N('playcard'):
        data = read_action_data()
        username, card_idx = struct.unpack('QB', data)
        playcard(username, card_idx)
    else:
        payer = receiver
        itr = users.find('helloo')
        print(itr)
        if itr >= 0:
            user = users.get(itr)
            print(user, user.game_data.deck_player)
        else:
            d = UserInfo('helloo')
            d.payer = receiver
            users.store(d)
