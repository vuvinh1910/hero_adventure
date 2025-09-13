#pragma once

#if defined(__aarch64__)
	auto RETURN = "CO 03 5F D6";
	auto NOP = "1F 20 03 D5";
	auto TRUE = "20 00 80 D2 CO 03 5F D6";
	auto FALSE = "00 00 80 D2 CO 03 5F D6";
	auto window_name = OBFUSCATE("Tuấn Meta | @ZTRChannel [X64]");
#else
	auto RETURN = "1E FF 2F E1";
	auto NOP = "00 F0 20 E3";
	auto TRUE = "01 00 A0 E3 1E FF 2F E1";
	auto FALSE = "00 00 A0 E3 1E FF 2F E1";
	auto window_name = OBFUSCATE("Tuấn Meta | @ZTRChannel [X32]");
#endif

int damage = 1, defense = 1, bonusExp = 1, pickUp = 1;
bool oneHit, dupItem, invisible, crit, freezeTime, x99_item, noCdKungFu, dumpEnemy, money, alwaysTurn, maxCreateCharacter, unlimitUseItem, fastLevelUpSkill;
uintptr_t battleTeam, action, battleActorProp, traitPoints, mainPropPoint, targetActor, itemInstance, m_stack;

enum class BattleTeamEnum {
    Player = 0,
    Enemy = 1,
    Allie = 2,
    Neutral = 3
};

enum class BattleActorPropTypes {
    Hp = 1,            // Máu hiện tại
    HpMax = 2,         // Máu tối đa
    Ep = 3,            // Năng lượng hiện tại
    EpMax = 4,         // Năng lượng tối đa

    // Kháng các loại trạng thái xấu
    NegPoison = 100,   // Kháng độc
    NegInjure = 101,   // Kháng thương
    NegSeal = 102,     // Kháng phong ấn
    NegBleed = 103,    // Kháng chảy máu
    NegBlind = 104,    // Kháng mù

    // Accuracy và Defense
    AccuDef = 200,     // Phòng thủ chính xác
    AccuAtk = 201,     // Tấn công chính xác
    AccuDefMax = 203,  // Phòng thủ chính xác tối đa

    // Battle Items
    BattleWine = 300,      // Rượu chiến đấu
    BattleMedicine = 301   // Thuốc chiến đấu
};

#pragma pack(push, 4)
struct Vector3Int {
    int32_t m_X; // 0x10
    int32_t m_Y; // 0x14
    int32_t m_Z; // 0x18
};
#pragma pack(pop)

bool (*_UnEquipItem)(void *instance, int type, bool needRecompute, bool returnToPack, void *pack);
bool UnEquipItem(void *instance, int type, bool needRecompute, bool returnToPack, void *pack) {
    if(instance) {
        if(dupItem) {
            return true;
        }
    }
    return _UnEquipItem(instance,type,needRecompute, returnToPack,pack);
}

void (*_Damage)(void *instance, int hp, void *caster);
void Damage(void *instance, int hp, void *caster) {
    if(instance) {
        BattleTeamEnum teamEnum = *(BattleTeamEnum *)((uintptr_t) instance + battleTeam);
        if(damage > 1) {
            if(teamEnum == BattleTeamEnum::Enemy || teamEnum == BattleTeamEnum::Neutral) {
                return _Damage(instance, hp*damage, caster);
            }
        }
        if(defense > 1) {
            if(teamEnum == BattleTeamEnum::Player || teamEnum == BattleTeamEnum::Allie) {
                return _Damage(instance, hp/defense, caster);
            }
        }
    }
    _Damage(instance,hp,caster);
}

void (*_set_isCirted)(void *instance, bool value);
void set_isCirted(void *instance, bool value) {
    if(instance) {
        void *target = *(void **) ((uintptr_t)instance + targetActor);
        if (target) {
            BattleTeamEnum targetTeam = *(BattleTeamEnum *)((uintptr_t) target + battleTeam);
            if(crit && (targetTeam == BattleTeamEnum::Enemy || targetTeam == BattleTeamEnum::Neutral)) {
                value = true;
            }
        }
    }
    _set_isCirted(instance, value);
}

void (*_set_isInstaKill)(void *instance, bool value);
void set_isInstaKill(void *instance, bool value) {
    if(instance) {
        void *target = *(void **) ((uintptr_t)instance + targetActor);
        if (target) {
            BattleTeamEnum targetTeam = *(BattleTeamEnum *)((uintptr_t) target + battleTeam);
            if(oneHit && (targetTeam == BattleTeamEnum::Enemy || targetTeam == BattleTeamEnum::Neutral)) {
                value = true;
            }
        }
    }
    _set_isInstaKill(instance, value);
}

void (*_BattleActorUpdate)(void *instance);
void BattleActorUpdate(void *instance) {
    if(instance) {
        if(alwaysTurn) {
            BattleTeamEnum teamEnum = *(BattleTeamEnum *)((uintptr_t) instance + battleTeam);
            if(teamEnum == BattleTeamEnum::Player || teamEnum == BattleTeamEnum::Allie) {
                void *actionProp = *(void **)((uintptr_t)instance + battleActorProp);
                *(float *)((uintptr_t)actionProp + action) = 999.9;
            } else {
                void *actionProp = *(void **)((uintptr_t)instance + battleActorProp);
                *(float *)((uintptr_t)actionProp + action) = -1.0;
            }
        }
    }
    _BattleActorUpdate(instance);
}

bool (*_SwitchTrait)(void *instance, void *trait, bool isOn);
bool SwitchTrait(void *instance, void *trait, bool isOn) {
    if(instance) {
        if(maxCreateCharacter) {
            *(int *) ((uintptr_t)instance + traitPoints) = 999999;
            *(int *) ((uintptr_t)instance + mainPropPoint) = 999;
        }
    }
    return _SwitchTrait(instance,trait,isOn);
}

long (*_GetCurrency)(void *instance, int type);
long GetCurrency(void *instance, int type) {
    if(instance && money) {
        return 999999999;
    }
    return _GetCurrency(instance, type);
}

bool (*_RemoveItem)(void *instance, void *gameItemInstance, int count);
bool RemoveItem(void *instance, void *gameItemInstance, int count) {
    if (instance && gameItemInstance) {
        if (unlimitUseItem) {
            return true;
        }
    }
    return _RemoveItem(instance, gameItemInstance, count);
}

int (*_get_DefaultExpToNextLevel)(void *instance);
int get_DefaultExpToNextLevel(void *instance) {
    if(instance) {
        if(fastLevelUpSkill) {
            return 1;
        }
    }
    return _get_DefaultExpToNextLevel(instance);
}

bool (*_AddItemById)(void *instance, int itemId, int count, bool needCheck);
bool (*_AddItemByData)(void *instance, void *itemData, int count, bool needCheck);

bool AddItemById(void *instance, int itemId, int count, bool needCheck) {
    if (instance) {
        if (pickUp > 1) {
            count *= pickUp;
        }
    }
    return _AddItemById(instance, itemId, count, needCheck);
}

bool AddItemByData(void *instance, void *itemData, int count, bool needCheck) {
    if (instance && itemData) {
        if (pickUp > 1) {
            count *= pickUp;
        }
    }
    return _AddItemByData(instance, itemData, count, needCheck);
}

int (*_GetKungfuCd)(void *instance, int kungFuId);
int GetKungfuCd(void *instance, int kungFuId) {
    if(instance) {
        BattleTeamEnum teamEnum = *(BattleTeamEnum *)((uintptr_t) instance + battleTeam);
        if(dumpEnemy) {
            if(teamEnum == BattleTeamEnum::Enemy || teamEnum == BattleTeamEnum::Neutral) {
                return 10;
            }
        }
        if(noCdKungFu) {
            if(teamEnum == BattleTeamEnum::Player || teamEnum == BattleTeamEnum::Allie) {
                return 0;
            }
        }
    }
    return _GetKungfuCd(instance, kungFuId);
}

int (*_GetExpInBattle)(void *instance, int origin, void *event, bool killBonus);
int GetExpInBattle(void *instance, int origin, void *event, bool killBonus) {
    if(instance) {
        if(bonusExp > 1) {
            return _GetExpInBattle(instance, origin*bonusExp, event, true);
        }
    }
    return _GetExpInBattle(instance, origin, event, killBonus);
}

void (*_OnPointerEnter)(void *instance, void *event);
void OnPointerEnter(void *instance, void *event) {
    if(instance) {
        if(x99_item) {
            void *temp = *(void **) ((uintptr_t) instance + itemInstance);
            if(temp) {
                *(int*) ((uintptr_t) temp + m_stack) = 99;
            }
        }
    }
    _OnPointerEnter(instance, event);
}

void (*_ApplyTime)(void *instance);
void ApplyTime(void *instance) {
    if(instance) {
        if(freezeTime) {
            return;
        }
    }
    _ApplyTime(instance);
}

bool (*_IsInRoleSight)(void *instance, void *role, Vector3Int point);
bool IsInRoleSight(void *instance, void *role, Vector3Int point) {
    if (instance && role) {
        if (invisible) {
            return false;
        }
    }
    return _IsInRoleSight(instance, role, point);
}