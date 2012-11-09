/*
 * Copyright (C) 2008 - 2011 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010 - 2012 Myth Project <http://mythprojectnetwork.blogspot.com/>
 *
 * Myth Project's source is based on the Trinity Project source, you can find the
 * link to that easily in Trinity Copyrights. Myth Project is a private community.
 * To get access, you either have to donate or pass a developer test.
 * You may not share Myth Project's sources! For personal use only.
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "Map.h"
#include "PoolMgr.h"
#include "icecrown_citadel.h"
#include "MapManager.h"
#include "Transport.h"

DoorData const doorData[] =
{
    {GO_LORD_MARROWGAR_S_ENTRANCE,           DATA_LORD_MARROWGAR,        DOOR_TYPE_ROOM,       BOUNDARY_N   },
    {GO_ICEWALL,                             DATA_LORD_MARROWGAR,        DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    {GO_DOODAD_ICECROWN_ICEWALL02,           DATA_LORD_MARROWGAR,        DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    {GO_ORATORY_OF_THE_DAMNED_ENTRANCE,      DATA_LADY_DEATHWHISPER,     DOOR_TYPE_ROOM,       BOUNDARY_N   },
    {GO_SAURFANG_S_DOOR,                     DATA_DEATHBRINGER_SAURFANG, DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    {GO_BLOODWING_DOOR,                      DATA_PROFESSOR_PUTRICIDE,   DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    {GO_FROSTWING_DOOR,                      DATA_BLOOD_QUEEN_LANA_THEL, DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    {GO_ORANGE_PLAGUE_MONSTER_ENTRANCE,      DATA_FESTERGUT,             DOOR_TYPE_ROOM,       BOUNDARY_E   },
    {GO_GREEN_PLAGUE_MONSTER_ENTRANCE,       DATA_ROTFACE,               DOOR_TYPE_ROOM,       BOUNDARY_E   },
    {GO_SCIENTIST_ENTRANCE,                  DATA_PROFESSOR_PUTRICIDE,   DOOR_TYPE_ROOM,       BOUNDARY_E   },
    {GO_CRIMSON_HALL_DOOR,                   DATA_BLOOD_PRINCE_COUNCIL,  DOOR_TYPE_ROOM,       BOUNDARY_S   },
    {GO_BLOOD_ELF_COUNCIL_DOOR,              DATA_BLOOD_PRINCE_COUNCIL,  DOOR_TYPE_PASSAGE,    BOUNDARY_W   },
    {GO_BLOOD_ELF_COUNCIL_DOOR_RIGHT,        DATA_BLOOD_PRINCE_COUNCIL,  DOOR_TYPE_PASSAGE,    BOUNDARY_E   },
    {GO_DOODAD_ICECROWN_BLOODPRINCE_DOOR_01, DATA_BLOOD_QUEEN_LANA_THEL, DOOR_TYPE_ROOM,       BOUNDARY_S   },
    {GO_DOODAD_ICECROWN_GRATE_01,            DATA_BLOOD_QUEEN_LANA_THEL, DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    {GO_GREEN_DRAGON_BOSS_ENTRANCE,          DATA_SISTER_SVALNA,         DOOR_TYPE_PASSAGE,    BOUNDARY_S   },
    {GO_GREEN_DRAGON_BOSS_ENTRANCE,          DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_ROOM,       BOUNDARY_N   },
    {GO_GREEN_DRAGON_BOSS_EXIT,              DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_PASSAGE,    BOUNDARY_S   },
    {GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_01,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE, BOUNDARY_N   },
    {GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_02,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE, BOUNDARY_S   },
    {GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_03,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE, BOUNDARY_N   },
    {GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_04,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE, BOUNDARY_S   },
    {GO_SINDRAGOSA_ENTRANCE_DOOR,            DATA_SINDRAGOSA,            DOOR_TYPE_ROOM,       BOUNDARY_S   },
    {GO_SINDRAGOSA_SHORTCUT_ENTRANCE_DOOR,   DATA_SINDRAGOSA,            DOOR_TYPE_PASSAGE,    BOUNDARY_E   },
    {GO_SINDRAGOSA_SHORTCUT_EXIT_DOOR,       DATA_SINDRAGOSA,            DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    {GO_ICE_WALL,                            DATA_SINDRAGOSA,            DOOR_TYPE_ROOM,       BOUNDARY_SE  },
    {GO_ICE_WALL,                            DATA_SINDRAGOSA,            DOOR_TYPE_ROOM,       BOUNDARY_SW  },
    {0,                                      0,                          DOOR_TYPE_ROOM,       BOUNDARY_NONE},// END
};

// this doesnt have to only store questgivers, also can be used for related quest spawns
struct WeeklyQuest
{
    uint32 creatureEntry;
    uint32 questId[2]; // 10 and 25 man versions
};

// when changing the content, remember to update SetData, DATA_BLOOD_QUICKENING_STATE case for NPC_ALRIN_THE_AGILE index
WeeklyQuest const WeeklyQuestData[WeeklyNPCs] =
{
    {NPC_INFILTRATOR_MINCHAR, {QUEST_DEPROGRAMMING_10, QUEST_DEPROGRAMMING_25 }}, // Deprogramming
    {NPC_KOR_KRON_LIEUTENANT, {QUEST_SECURING_THE_RAMPARTS_10, QUEST_SECURING_THE_RAMPARTS_25 }}, // Securing the Ramparts
    {NPC_ROTTING_FROST_GIANT_10, {QUEST_SECURING_THE_RAMPARTS_10, QUEST_SECURING_THE_RAMPARTS_25 }}, // Securing the Ramparts
    {NPC_ROTTING_FROST_GIANT_25, {QUEST_SECURING_THE_RAMPARTS_10, QUEST_SECURING_THE_RAMPARTS_25 }}, // Securing the Ramparts
    {NPC_ALCHEMIST_ADRIANNA, {QUEST_RESIDUE_RENDEZVOUS_10, QUEST_RESIDUE_RENDEZVOUS_25 }}, // Residue Rendezvous
    {NPC_ALRIN_THE_AGILE, {QUEST_BLOOD_QUICKENING_10, QUEST_BLOOD_QUICKENING_25 }}, // Blood Quickening
    {NPC_INFILTRATOR_MINCHAR_BQ, {QUEST_BLOOD_QUICKENING_10, QUEST_BLOOD_QUICKENING_25 }}, // Blood Quickening
    {NPC_MINCHAR_BEAM_STALKER, {QUEST_BLOOD_QUICKENING_10, QUEST_BLOOD_QUICKENING_25 }}, // Blood Quickening
    {NPC_VALITHRIA_DREAMWALKER_QUEST, {QUEST_RESPITE_FOR_A_TORNMENTED_SOUL_10, QUEST_RESPITE_FOR_A_TORNMENTED_SOUL_25}}, // Respite for a Tormented Soul
};

class instance_icecrown_citadel : public InstanceMapScript
{
    public:
        instance_icecrown_citadel() : InstanceMapScript(ICCScriptName, 631) { }

        struct instance_icecrown_citadel_InstanceMapScript : public InstanceScript
        {
            instance_icecrown_citadel_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);
                TeamInInstance                  = 0;
                HeroicAttempts                  = MaxHeroicAttempts;
                LadyDeathwisperElevatorGUID     = 0;
                // Gunship Battle
                FirstSquadState = 0;
                SecondSquadState = 0;
                SpireSquadState = 0;
                SkybreakerBossGUID = 0;
                OrgrimmarBossGUID = 0;
                DeathbringerSaurfangGbGUID = 0;
                MuradinBronzebeardGbGUID = 0;
                DeathbringerSaurfangNotVisualGUID = 0;
                MuradinBronzebeardNotVisualGUID = 0;
                GbBattleMageGUID = 0;
                // Gunship Battle
                DeathbringerSaurfangGUID        = 0;
                DeathbringerSaurfangDoorGUID    = 0;
                DeathbringerSaurfangEventGUID   = 0;
                DeathbringersCacheGUID          = 0;
                BloodwingDoor                   = 0;
                FrostwingDoor                   = 0;
                SaurfangTeleportGUID            = 0;
                PlagueSigilGUID                 = 0;
                BloodwingSigilGUID              = 0;
                FrostwingSigilGUID              = 0;
                memset(PutricidePipeGUIDs, 0, 2 * sizeof(uint64));
                memset(PutricideGateGUIDs, 0, 2 * sizeof(uint64));
                PutricideCollisionGUID          = 0;
                FestergutGUID                   = 0;
                RotfaceGUID                     = 0;
                ProfessorPutricideGUID          = 0;
                PutricideTableGUID              = 0;
                memset(BloodCouncilGUIDs, 0, 3 * sizeof(uint64));
                BloodCouncilControllerGUID      = 0;
                BloodQueenLanaThelGUID          = 0;
                CrokScourgebaneGUID             = 0;
                memset(CrokCaptainGUIDs, 0, 4 * sizeof(uint64));
                SisterSvalnaGUID                = 0;
                ValithriaDreamwalkerGUID        = 0;
                ValithriaLichKingGUID           = 0;
                ValithriaTriggerGUID            = 0;
                SindragosaGUID                  = 0;
                SpinestalkerGUID                = 0;
                RimefangGUID                    = 0;
                uiDreamwalkerCache              = 0;
                TheLichKingGUID                 = 0;
                uiTirion                        = 0;
                uiTerenasFighter                = 0;
                uiSpiritWarden                  = 0;
                uiIceShard1                     = 0;
                uiIceShard2                     = 0;
                uiIceShard3                     = 0;
                uiIceShard4                     = 0;
                uiFrostyEdgeInner               = 0;
                uiFrostyEdgeOuter               = 0;
                uiEdgeDestroyWarning            = 0;
                uilavaman                       = 0;
                uihangingman                    = 0;
                IsNeckDeep                      = true;
                IsNecroticStack                 = true;
                FrostwyrmCount                  = 0;
                SpinestalkerTrashCount          = 0;
                RimefangTrashCount              = 0;
                IsBonedEligible                 = true;
                IsOozeDanceEligible             = true;
                IsNauseaEligible                = true;
                IsOrbWhispererEligible          = true;
                ColdflameJetsState              = NOT_STARTED;
                BloodQuickeningState            = NOT_STARTED;
                SindragosaGauntlet = NOT_STARTED;
                BloodQuickeningTimer            = 0;
                BloodQuickeningMinutes          = 0;
                ZoneBuffState = 0;
                isPrepared = false;
                SindragosasWardGUID = 0;
                SindragosaGauntletDoorGUID = 0;
            }

            void FillInitialWorldStates(WorldPacket& data)
            {
                data << uint32(WORLDSTATE_SHOW_TIMER) << uint32(BloodQuickeningState == IN_PROGRESS);
                data << uint32(WORLDSTATE_EXECUTION_TIME) << uint32(BloodQuickeningMinutes);
                data << uint32(WORLDSTATE_SHOW_ATTEMPTS) << uint32(instance->IsHeroic());
                data << uint32(WORLDSTATE_ATTEMPTS_REMAINING) << uint32(HeroicAttempts);
                data << uint32(WORLDSTATE_ATTEMPTS_MAX) << uint32(MaxHeroicAttempts);
            }

            void OnPlayerEnter(Player* pPlayer)
            {
                if(!TeamInInstance)
                    TeamInInstance = pPlayer->GetTeam();

                if(GetData(DATA_ZONE_BUFF_STATUS) == 1)
                {
                    if(pPlayer->HasAura(AURA_ZONE_PCT_BUFF_A_10))
                        DoRemoveAurasDueToSpellOnPlayers(AURA_ZONE_PCT_BUFF_A_10);
                    else if(pPlayer->HasAura(AURA_ZONE_PCT_BUFF_H_10))
                        DoRemoveAurasDueToSpellOnPlayers(AURA_ZONE_PCT_BUFF_H_10);
                }

                PrepareGunshipEvent(); // Spawn Gunship Event
                if(TeamInInstance)
                {
                    switch(TeamInInstance)
                    {
                        case ALLIANCE:
                            if(pPlayer && pPlayer->IsInWorld())
                                pPlayer->AddAura(73828, pPlayer);
                            break;
                        case HORDE:
                            if(pPlayer && pPlayer->IsInWorld())
                                pPlayer->AddAura(73822, pPlayer);
                            break;
                    }
                }
            }

            void OnCreatureCreate(Creature* creature)
            {
                if(!TeamInInstance)
                {
                    Map::PlayerList const &players = instance->GetPlayers();
                    if(!players.isEmpty())
                        if(Player* player = players.begin()->getSource())
                            TeamInInstance = player->GetTeam();
                }

                switch(creature->GetEntry())
                {
                    case NPC_GB_SKYBREAKER:
                        SkybreakerBossGUID = creature->GetGUID();
                        break;
                    case NPC_GB_ORGRIMS_HAMMER:
                        OrgrimmarBossGUID = creature->GetGUID();
                        break;
                    case NPC_GB_HIGH_OVERLORD_SAURFANG:
                        DeathbringerSaurfangGbGUID = creature->GetGUID();
                        break;
                    case NPC_GB_MURADIN_BRONZEBEARD:
                        MuradinBronzebeardGbGUID = creature->GetGUID();
                        break;
                    case NPC_GB_HIGH_OVERLORD_SAURFANG_NOT_VISUAL:
                        DeathbringerSaurfangNotVisualGUID = creature->GetGUID();
                        break;
                    case NPC_GB_MURADIN_BRONZEBEARD_NOT_VISUAL:
                        MuradinBronzebeardNotVisualGUID = creature->GetGUID();
                        break;
                    case NPC_GB_SKYBREAKER_SORCERERS:
                    case NPC_GB_KORKRON_BATTLE_MAGE:
                        GbBattleMageGUID = creature->GetGUID();
                        break;
                    case NPC_KOR_KRON_GENERAL:
                        if(TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_ALLIANCE_COMMANDER, ALLIANCE);
                        break;
                    case NPC_KOR_KRON_LIEUTENANT:
                        if(TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_SKYBREAKER_LIEUTENANT, ALLIANCE);
                        break;
                    case NPC_TORTUNOK:
                        if(TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_ALANA_MOONSTRIKE, ALLIANCE);
                        break;
                    case NPC_GERARDO_THE_SUAVE:
                        if(TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_TALAN_MOONSTRIKE, ALLIANCE);
                        break;
                    case NPC_UVLUS_BANEFIRE:
                        if(TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_MALFUS_GRIMFROST, ALLIANCE);
                        break;
                    case NPC_IKFIRUS_THE_VILE:
                        if(TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_YILI, ALLIANCE);
                        break;
                    case NPC_VOL_GUK:
                        if(TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_JEDEBIA, ALLIANCE);
                        break;
                    case NPC_HARAGG_THE_UNSEEN:
                        if(TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_NIBY_THE_ALMIGHTY, ALLIANCE);
                        break;
                    case NPC_GARROSH_HELLSCREAM:
                        if(TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_KING_VARIAN_WRYNN, ALLIANCE);
                        break;
                    case NPC_DEATHBRINGER_SAURFANG:
                        DeathbringerSaurfangGUID = creature->GetGUID();
                        break;
                    case NPC_SE_HIGH_OVERLORD_SAURFANG:
                        if(TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_SE_MURADIN_BRONZEBEARD, ALLIANCE, creature->GetCreatureData());
                        // no break;
                    case NPC_SE_MURADIN_BRONZEBEARD:
                        DeathbringerSaurfangEventGUID = creature->GetGUID();
                        creature->LastUsedScriptID = creature->GetScriptId();
                        break;
                    case NPC_SE_KOR_KRON_REAVER:
                        if(TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_SE_SKYBREAKER_MARINE, ALLIANCE);
                        break;
                    case NPC_FROST_FREEZE_TRAP:
                        ColdflameJetGUIDs.insert(creature->GetGUID());
                        break;
                    case NPC_FESTERGUT:
                        FestergutGUID = creature->GetGUID();
                        break;
                    case NPC_ROTFACE:
                        RotfaceGUID = creature->GetGUID();
                        break;
                    case NPC_PROFESSOR_PUTRICIDE:
                        ProfessorPutricideGUID = creature->GetGUID();
                        break;
                    case NPC_PRINCE_KELESETH:
                        BloodCouncilGUIDs[0] = creature->GetGUID();
                        break;
                    case NPC_PRINCE_TALDARAM:
                        BloodCouncilGUIDs[1] = creature->GetGUID();
                        break;
                    case NPC_PRINCE_VALANAR:
                        BloodCouncilGUIDs[2] = creature->GetGUID();
                        break;
                    case NPC_BLOOD_ORB_CONTROLLER:
                        BloodCouncilControllerGUID = creature->GetGUID();
                        break;
                    case NPC_BLOOD_QUEEN_LANA_THEL:
                        BloodQueenLanaThelGUID = creature->GetGUID();
                        break;
                    case NPC_CROK_SCOURGEBANE:
                        CrokScourgebaneGUID = creature->GetGUID();
                        break;
                    // we can only do this because there are no gaps in their entries
                    case NPC_CAPTAIN_ARNATH:
                    case NPC_CAPTAIN_BRANDON:
                    case NPC_CAPTAIN_GRONDEL:
                    case NPC_CAPTAIN_RUPERT:
                        CrokCaptainGUIDs[creature->GetEntry()-NPC_CAPTAIN_ARNATH] = creature->GetGUID();
                        break;
                    case NPC_SISTER_SVALNA:
                        SisterSvalnaGUID = creature->GetGUID();
                        break;
                    case NPC_VALITHRIA_DREAMWALKER:
                        ValithriaDreamwalkerGUID = creature->GetGUID();
                        break;
                    case NPC_THE_LICH_KING_VALITHRIA:
                        ValithriaLichKingGUID = creature->GetGUID();
                        break;
                    case NPC_GREEN_DRAGON_COMBAT_TRIGGER:
                        ValithriaTriggerGUID = creature->GetGUID();
                        break;
                    case NPC_SINDRAGOSA:
                        SindragosaGUID = creature->GetGUID();
                        break;
                    case NPC_SPINESTALKER:
                        SpinestalkerGUID = creature->GetGUID();
                        if(!creature->isDead())
                            ++FrostwyrmCount;
                        break;
                    case NPC_RIMEFANG:
                        RimefangGUID = creature->GetGUID();
                        if(!creature->isDead())
                            ++FrostwyrmCount;
                        break;
                    case NPC_THE_LICH_KING:
                        TheLichKingGUID = creature->GetGUID();
                        break;
                    case NPC_TIRION_ICC:
                        uiTirion = creature->GetGUID();
                        break;
                    case NPC_TERENAS_FIGHTER:
                        uiTerenasFighter = creature->GetGUID();
                        break;
                    case NPC_SPIRIT_WARDEN:
                        uiSpiritWarden = creature->GetGUID();
                        break;
                    case NPC_SINDRAGOSAS_WARD:
                        SindragosasWardGUID = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            // Weekly quest spawn prevention
            uint32 GetCreatureEntry(uint32 /*guidLow*/, CreatureData const* data)
            {
                uint32 entry = data->id;
                switch(entry)
                {
                    case NPC_INFILTRATOR_MINCHAR:
                    case NPC_KOR_KRON_LIEUTENANT:
                    case NPC_ALCHEMIST_ADRIANNA:
                    case NPC_ALRIN_THE_AGILE:
                    case NPC_INFILTRATOR_MINCHAR_BQ:
                    case NPC_MINCHAR_BEAM_STALKER:
                    case NPC_VALITHRIA_DREAMWALKER_QUEST:
                    {
                        for(uint8 questIndex = 0; questIndex < WeeklyNPCs; ++questIndex)
                        {
                            if(WeeklyQuestData[questIndex].creatureEntry == entry)
                            {
                                uint8 diffIndex = uint8(instance->GetSpawnMode() & 1);
                                if(!sPoolMgr->IsSpawnedObject<Quest>(WeeklyQuestData[questIndex].questId[diffIndex]))
                                    entry = 0;
                                break;
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }

                return entry;
            }

            void OnCreatureRemove(Creature* creature)
            {
                if(creature->GetEntry() == NPC_FROST_FREEZE_TRAP)
                    ColdflameJetGUIDs.erase(creature->GetGUID());
            }

            void OnCreatureDeath(Creature* creature)
            {
                switch(creature->GetEntry())
                {
                    case NPC_YMIRJAR_BATTLE_MAIDEN:
                    case NPC_YMIRJAR_DEATHBRINGER:
                    case NPC_YMIRJAR_FROSTBINDER:
                    case NPC_YMIRJAR_HUNTRESS:
                    case NPC_YMIRJAR_WARLORD:
                        if(Creature* crok = instance->GetCreature(CrokScourgebaneGUID))
                            crok->AI()->SetGUID(creature->GetGUID(), ACTION_VRYKUL_DEATH);
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go)
            {
                switch(go->GetEntry())
                {
                    case GO_DOODAD_ICECROWN_ICEWALL02:
                    case GO_ICEWALL:
                    case GO_LORD_MARROWGAR_S_ENTRANCE:
                    case GO_ORATORY_OF_THE_DAMNED_ENTRANCE:
                    case GO_ORANGE_PLAGUE_MONSTER_ENTRANCE:
                    case GO_GREEN_PLAGUE_MONSTER_ENTRANCE:
                    case GO_SCIENTIST_ENTRANCE:
                    case GO_CRIMSON_HALL_DOOR:
                    case GO_BLOOD_ELF_COUNCIL_DOOR:
                    case GO_BLOOD_ELF_COUNCIL_DOOR_RIGHT:
                    case GO_DOODAD_ICECROWN_BLOODPRINCE_DOOR_01:
                    case GO_DOODAD_ICECROWN_GRATE_01:
                    case GO_GREEN_DRAGON_BOSS_ENTRANCE:
                    case GO_GREEN_DRAGON_BOSS_EXIT:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_02:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_03:
                    case GO_SINDRAGOSA_SHORTCUT_ENTRANCE_DOOR:
                    case GO_SINDRAGOSA_SHORTCUT_EXIT_DOOR:
                    case GO_ICE_WALL:
                        AddDoor(go, true);
                        break;
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_01:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_04:
                        if(instance->GetSpawnMode() & 1)
                            AddDoor(go, true);
                        break;
                    case GO_LADY_DEATHWHISPER_ELEVATOR:
                        LadyDeathwisperElevatorGUID = go->GetGUID();
                        if(GetBossState(DATA_LADY_DEATHWHISPER) == DONE)
                        {
                            go->SetUInt32Value(GAMEOBJECT_LEVEL, 0);
                            go->SetGoState(GO_STATE_READY);
                        }
                        break;
                    case GO_SAURFANG_S_DOOR:
                        DeathbringerSaurfangDoorGUID = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_BLOODWING_DOOR:
                        BloodwingDoor = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_FROSTWING_DOOR:
                        FrostwingDoor = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_DEATHBRINGER_S_CACHE_10N:
                    case GO_DEATHBRINGER_S_CACHE_25N:
                    case GO_DEATHBRINGER_S_CACHE_10H:
                    case GO_DEATHBRINGER_S_CACHE_25H:
                        DeathbringersCacheGUID = go->GetGUID();
                        break;
                    case GO_SCOURGE_TRANSPORTER_SAURFANG:
                        SaurfangTeleportGUID = go->GetGUID();
                        break;
                    case GO_PLAGUE_SIGIL:
                        PlagueSigilGUID = go->GetGUID();
                        if(GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE)
                            HandleGameObject(PlagueSigilGUID, false, go);
                        break;
                    case GO_BLOODWING_SIGIL:
                        BloodwingSigilGUID = go->GetGUID();
                        if(GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE)
                            HandleGameObject(BloodwingSigilGUID, false, go);
                        break;
                    case GO_SIGIL_OF_THE_FROSTWING:
                        FrostwingSigilGUID = go->GetGUID();
                        if(GetBossState(DATA_SINDRAGOSA) == DONE)
                            HandleGameObject(FrostwingSigilGUID, false, go);
                        break;
                    case GO_SCIENTIST_AIRLOCK_DOOR_COLLISION:
                        PutricideCollisionGUID = go->GetGUID();
                        if(GetBossState(DATA_FESTERGUT) == DONE && GetBossState(DATA_ROTFACE) == DONE)
                            HandleGameObject(PutricideCollisionGUID, true, go);
                        break;
                    case GO_SCIENTIST_AIRLOCK_DOOR_ORANGE:
                        PutricideGateGUIDs[0] = go->GetGUID();
                        if(GetBossState(DATA_FESTERGUT) == DONE && GetBossState(DATA_ROTFACE) == DONE)
                            go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        else if(GetBossState(DATA_FESTERGUT) == DONE)
                            HandleGameObject(PutricideGateGUIDs[1], false, go);
                        break;
                    case GO_SCIENTIST_AIRLOCK_DOOR_GREEN:
                        PutricideGateGUIDs[1] = go->GetGUID();
                        if(GetBossState(DATA_ROTFACE) == DONE && GetBossState(DATA_FESTERGUT) == DONE)
                            go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        else if(GetBossState(DATA_ROTFACE) == DONE)
                            HandleGameObject(PutricideGateGUIDs[1], false, go);
                        break;
                    case GO_DOODAD_ICECROWN_ORANGETUBES02:
                        PutricidePipeGUIDs[0] = go->GetGUID();
                        if(GetBossState(DATA_FESTERGUT) == DONE)
                            HandleGameObject(PutricidePipeGUIDs[0], true, go);
                        break;
                    case GO_DOODAD_ICECROWN_GREENTUBES02:
                        PutricidePipeGUIDs[1] = go->GetGUID();
                        if(GetBossState(DATA_ROTFACE) == DONE)
                            HandleGameObject(PutricidePipeGUIDs[1], true, go);
                        break;
                    case GO_DRINK_ME:
                        PutricideTableGUID = go->GetGUID();
                        break;
                    case GO_DREAMWALKER_CACHE_10_N:
                    case GO_DREAMWALKER_CACHE_25_N:
                    case GO_DREAMWALKER_CACHE_10_H:
                    case GO_DREAMWALKER_CACHE_25_H:
                        uiDreamwalkerCache = go->GetGUID();
                        if(Creature* valithria = instance->GetCreature(ValithriaDreamwalkerGUID))
                            go->SetLootRecipient(valithria->GetLootRecipient());
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);
                        break;
                    //Lich King
                    case GO_LAVAMAN:
                        uilavaman = go->GetGUID();
                        if(GetBossState(DATA_THE_LICH_KING) == DONE)
                            go->SetRespawnTime(5*DAY);
                        break;
                    case GO_HANGINGMAN:
                        uihangingman = go->GetGUID();
                        break;
                    case GO_ICE_SHARD_1:
                        uiIceShard1 = go->GetGUID();
                        go->SetGoState(GetBossState(DATA_THE_LICH_KING) == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                        break;
                    case GO_ICE_SHARD_2:
                        uiIceShard2 = go->GetGUID();
                        go->SetGoState(GetBossState(DATA_THE_LICH_KING) == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                        break;
                    case GO_ICE_SHARD_3:
                        uiIceShard3 = go->GetGUID();
                        go->SetGoState(GetBossState(DATA_THE_LICH_KING) == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                        break;
                    case GO_ICE_SHARD_4:
                        uiIceShard4 = go->GetGUID();
                        go->SetGoState(GetBossState(DATA_THE_LICH_KING) == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                        break;
                    case GO_FROSTY_EDGE_OUTER:
                        uiFrostyEdgeOuter = go->GetGUID();
                        go->SetGoState(GO_STATE_ACTIVE);
                        break;
                    case GO_FROSTY_EDGE_INNER:
                        uiFrostyEdgeInner = go->GetGUID();
                        go->SetGoState(GO_STATE_READY);
                        break;
                    case GO_EDGE_DESTROY_WARNING:
                        uiEdgeDestroyWarning = go->GetGUID();
                        go->SetGoState(GO_STATE_READY);
                        break;
                    case GO_SINDRAGOSA_ENTRANCE_DOOR:
                        if(GetData(DATA_SINDRAGOSAS_GAUNTLET) == DONE)
                            go->SetGoState(GO_STATE_READY);
                        SindragosaGauntletDoorGUID = go->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go)
            {
                switch(go->GetEntry())
                {
                    case GO_DOODAD_ICECROWN_ICEWALL02:
                    case GO_ICEWALL:
                    case GO_LORD_MARROWGAR_S_ENTRANCE:
                    case GO_ORATORY_OF_THE_DAMNED_ENTRANCE:
                    case GO_SAURFANG_S_DOOR:
                    case GO_FROSTWING_DOOR:
                    case GO_BLOODWING_DOOR:
                    case GO_ORANGE_PLAGUE_MONSTER_ENTRANCE:
                    case GO_GREEN_PLAGUE_MONSTER_ENTRANCE:
                    case GO_SCIENTIST_ENTRANCE:
                    case GO_CRIMSON_HALL_DOOR:
                    case GO_BLOOD_ELF_COUNCIL_DOOR:
                    case GO_BLOOD_ELF_COUNCIL_DOOR_RIGHT:
                    case GO_DOODAD_ICECROWN_BLOODPRINCE_DOOR_01:
                    case GO_DOODAD_ICECROWN_GRATE_01:
                    case GO_GREEN_DRAGON_BOSS_ENTRANCE:
                    case GO_GREEN_DRAGON_BOSS_EXIT:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_01:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_02:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_03:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_04:
                    case GO_SINDRAGOSA_ENTRANCE_DOOR:
                    case GO_SINDRAGOSA_SHORTCUT_ENTRANCE_DOOR:
                    case GO_SINDRAGOSA_SHORTCUT_EXIT_DOOR:
                    case GO_ICE_WALL:
                        AddDoor(go, false);
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 type)
            {
                switch(type)
                {
                    case DATA_FIRST_SQUAD_STATE:
                        return FirstSquadState;
                    case DATA_SECOND_SQUAD_STATE:
                        return SecondSquadState;
                    case DATA_SPIRE_FROSTWYRM_STATE:
                        return SpireSquadState;
                    case DATA_SINDRAGOSA_FROSTWYRMS:
                        return FrostwyrmCount;
                    case DATA_SPINESTALKER:
                        return SpinestalkerTrashCount;
                    case DATA_RIMEFANG:
                        return RimefangTrashCount;
                    case DATA_COLDFLAME_JETS:
                        return ColdflameJetsState;
                    case DATA_TEAM_IN_INSTANCE:
                        return TeamInInstance;
                    case DATA_BLOOD_QUICKENING_STATE:
                        return BloodQuickeningState;
                    case DATA_HEROIC_ATTEMPTS:
                        return HeroicAttempts;
                    case DATA_ZONE_BUFF_STATUS:
                        return ZoneBuffState;
                    case DATA_SINDRAGOSAS_GAUNTLET:
                        return SindragosaGauntlet;
                    default:
                        break;
                }

                return 0;
            }

            uint64 GetData64(uint32 type)
            {
                switch(type)
                {
                    //Gunship battle
                    case DATA_SKYBREAKER_BOSS:
                        return SkybreakerBossGUID;
                    case DATA_ORGRIMMAR_HAMMER_BOSS:
                        return OrgrimmarBossGUID;
                    case DATA_GB_HIGH_OVERLORD_SAURFANG:
                        return DeathbringerSaurfangGbGUID;
                    case DATA_GB_MURADIN_BRONZEBEARD:
                        return MuradinBronzebeardGbGUID;
                    case DATA_HIGH_OVERLORD_SAURFANG_NOT_VISUAL:
                        return DeathbringerSaurfangNotVisualGUID;
                    case DATA_MURADIN_BRONZEBEARD_NOT_VISUAL:
                        return MuradinBronzebeardNotVisualGUID;
                    case DATA_GB_BATTLE_MAGE:
                        return GbBattleMageGUID;
                    case GUID_ICE_SHARD_1:
                        return uiIceShard1;
                    case GUID_ICE_SHARD_2:
                        return uiIceShard2;
                    case GUID_ICE_SHARD_3:
                        return uiIceShard3;
                    case GUID_ICE_SHARD_4:
                        return uiIceShard4;
                    case GUID_FROSTY_EDGE_OUTER:
                        return uiFrostyEdgeOuter;
                    case GUID_FROSTY_EDGE_INNER:
                        return uiFrostyEdgeInner;
                    case GUID_EDGE_DESTROY_WARNING:
                        return uiEdgeDestroyWarning;
                    case GUID_LAVAMAN:
                        return uilavaman;
                    case GUID_HANGINGMAN:
                        return uihangingman;
                    case GUID_TIRION:
                           return uiTirion;
                    case GUID_TERENAS_FIGHTER:
                        return uiTerenasFighter;
                    case GUID_SPIRIT_WARDEN:
                        return uiSpiritWarden;
                    case DATA_DEATHBRINGER_SAURFANG:
                        return DeathbringerSaurfangGUID;
                    case DATA_SAURFANG_EVENT_NPC:
                        return DeathbringerSaurfangEventGUID;
                    case GO_BLOODWING_DOOR:
                        return BloodwingDoor;
                    case GO_FROSTWING_DOOR:
                        return FrostwingDoor;
                    case GO_SAURFANG_S_DOOR:
                        return DeathbringerSaurfangDoorGUID;
                    case GO_SCOURGE_TRANSPORTER_SAURFANG:
                        return SaurfangTeleportGUID;
                    case DATA_FESTERGUT:
                        return FestergutGUID;
                    case DATA_ROTFACE:
                        return RotfaceGUID;
                    case DATA_PROFESSOR_PUTRICIDE:
                        return ProfessorPutricideGUID;
                    case DATA_PUTRICIDE_TABLE:
                        return PutricideTableGUID;
                    case DATA_PRINCE_KELESETH_GUID:
                        return BloodCouncilGUIDs[0];
                    case DATA_PRINCE_TALDARAM_GUID:
                        return BloodCouncilGUIDs[1];
                    case DATA_PRINCE_VALANAR_GUID:
                        return BloodCouncilGUIDs[2];
                    case DATA_BLOOD_PRINCES_CONTROL:
                        return BloodCouncilControllerGUID;
                    case DATA_BLOOD_QUEEN_LANA_THEL:
                        return BloodQueenLanaThelGUID;
                    case DATA_CROK_SCOURGEBANE:
                        return CrokScourgebaneGUID;
                    case DATA_CAPTAIN_ARNATH:
                    case DATA_CAPTAIN_BRANDON:
                    case DATA_CAPTAIN_GRONDEL:
                    case DATA_CAPTAIN_RUPERT:
                        return CrokCaptainGUIDs[type - DATA_CAPTAIN_ARNATH];
                    case DATA_SISTER_SVALNA:
                        return SisterSvalnaGUID;
                    case DATA_VALITHRIA_DREAMWALKER:
                        return ValithriaDreamwalkerGUID;
                    case DATA_VALITHRIA_LICH_KING:
                        return ValithriaLichKingGUID;
                    case DATA_VALITHRIA_TRIGGER:
                        return ValithriaTriggerGUID;
                    case DATA_SINDRAGOSA:
                        return SindragosaGUID;
                    case DATA_SPINESTALKER:
                        return SpinestalkerGUID;
                    case DATA_RIMEFANG:
                        return RimefangGUID;
                    case DATA_THE_LICH_KING:
                        return TheLichKingGUID;
                    case GUID_SINDRAGOSAS_WARD:
                        return SindragosasWardGUID;
                    default:
                        break;
                }

                return 0;
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                if(!InstanceScript::SetBossState(type, state))
                    return false;

                switch(type)
                {
                    case DATA_LADY_DEATHWHISPER:
                        //SetBossState(DATA_GUNSHIP_EVENT, state); // TEMP HACK UNTIL GUNSHIP SCRIPTED
                        if(state == DONE)
                        {
                            if(GameObject* elevator = instance->GetGameObject(LadyDeathwisperElevatorGUID))
                            {
                                elevator->SetUInt32Value(GAMEOBJECT_LEVEL, 0);
                                elevator->SetGoState(GO_STATE_READY);
                            }
                        }
                        break;
                    case DATA_DEATHBRINGER_SAURFANG:
                        switch(state)
                        {
                            case NOT_STARTED:
                                if(GameObject* teleporter = instance->GetGameObject(SaurfangTeleportGUID))
                                {
                                    HandleGameObject(SaurfangTeleportGUID, true, teleporter);
                                    teleporter->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    case DATA_FESTERGUT:
                        if(state == DONE)
                        {
                            if(GetBossState(DATA_ROTFACE) == DONE)
                            {
                                HandleGameObject(PutricideCollisionGUID, true);
                                if(GameObject* go = instance->GetGameObject(PutricideGateGUIDs[0]))
                                    go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                                if(GameObject* go = instance->GetGameObject(PutricideGateGUIDs[1]))
                                    go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                            }
                            else
                                HandleGameObject(PutricideGateGUIDs[0], false);
                            HandleGameObject(PutricidePipeGUIDs[0], true);
                        }
                        break;
                    case DATA_ROTFACE:
                        if(state == DONE)
                        {
                            if(GetBossState(DATA_FESTERGUT) == DONE)
                            {
                                HandleGameObject(PutricideCollisionGUID, true);
                                if(GameObject* go = instance->GetGameObject(PutricideGateGUIDs[0]))
                                    go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                                if(GameObject* go = instance->GetGameObject(PutricideGateGUIDs[1]))
                                    go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                            }
                            else
                                HandleGameObject(PutricideGateGUIDs[1], false);
                            HandleGameObject(PutricidePipeGUIDs[1], true);
                        }
                        break;
                    case DATA_PROFESSOR_PUTRICIDE:
                        HandleGameObject(PlagueSigilGUID, state != DONE);
                        if(instance->IsHeroic())
                        {
                            if(state == FAIL && HeroicAttempts)
                            {
                                --HeroicAttempts;
                                DoUpdateWorldState(WORLDSTATE_ATTEMPTS_REMAINING, HeroicAttempts);
                                if(!HeroicAttempts)
                                    if(Creature* putricide = instance->GetCreature(ProfessorPutricideGUID))
                                        putricide->DespawnOrUnsummon();
                            }
                        }
                        break;
                    case DATA_BLOOD_QUEEN_LANA_THEL:
                        HandleGameObject(BloodwingSigilGUID, state != DONE);
                        if(instance->IsHeroic())
                        {
                            if(state == FAIL && HeroicAttempts)
                            {
                                --HeroicAttempts;
                                DoUpdateWorldState(WORLDSTATE_ATTEMPTS_REMAINING, HeroicAttempts);
                                if(!HeroicAttempts)
                                    if(Creature* bq = instance->GetCreature(BloodQueenLanaThelGUID))
                                        bq->DespawnOrUnsummon();
                            }
                        }
                        break;
                    case DATA_VALITHRIA_DREAMWALKER:
                        if(state == DONE && sPoolMgr->IsSpawnedObject<Quest>(WeeklyQuestData[8].questId[instance->GetSpawnMode() & 1]))
                            instance->SummonCreature(NPC_VALITHRIA_DREAMWALKER_QUEST, ValithriaSpawnPos);

                        if(state == DONE)
                        {
                            if(GameObject* pChest = instance->GetGameObject(uiDreamwalkerCache))
                                pChest->SetRespawnTime(pChest->GetRespawnDelay());
                        }
                        break;
                    case DATA_SINDRAGOSA:
                        HandleGameObject(FrostwingSigilGUID, state != DONE);
                        if(instance->IsHeroic())
                        {
                            if(state == FAIL && HeroicAttempts)
                            {
                                --HeroicAttempts;
                                DoUpdateWorldState(WORLDSTATE_ATTEMPTS_REMAINING, HeroicAttempts);
                                if(!HeroicAttempts)
                                    if(Creature* sindra = instance->GetCreature(SindragosaGUID))
                                        sindra->DespawnOrUnsummon();
                            }
                        }
                        break;
                    case DATA_THE_LICH_KING:
                        if(instance->IsHeroic())
                        {
                            if(state == FAIL && HeroicAttempts)
                            {
                                --HeroicAttempts;
                                DoUpdateWorldState(WORLDSTATE_ATTEMPTS_REMAINING, HeroicAttempts);
                                if(!HeroicAttempts)
                                    if(Creature* theLichKing = instance->GetCreature(TheLichKingGUID))
                                        theLichKing->DespawnOrUnsummon();
                            }
                        }
                        switch(state)
                        {
                            case NOT_STARTED:
                            {
                                if(GameObject* go = instance->GetGameObject(uilavaman))
                                    go->SetPhaseMask(2, true);
                            }
                                break;
                            case DONE:
                            {
                                if(GameObject* go = instance->GetGameObject(uilavaman))
                                    go->SetPhaseMask(1, true);
                                if(GameObject* go = instance->GetGameObject(uilavaman))
                                    go->SetRespawnTime(5*DAY);;
                                if(GameObject* go = instance->GetGameObject(uihangingman))
                                    go->SetPhaseMask(2, true);
                            }
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                 }

                 return true;
            }

            void SetData(uint32 type, uint32 data)
            {
                switch(type)
                {
                    case DATA_NECK_DEEP_ACHIEVEMENT:
                        IsNeckDeep = data ? true : false;
                        break;
                    case DATA_BEEN_WAITING_ACHIEVEMENT:
                        IsNecroticStack = data ? true : false;
                        break;
                    case DATA_BONED_ACHIEVEMENT:
                        IsBonedEligible = data ? true : false;
                        break;
                    case DATA_OOZE_DANCE_ACHIEVEMENT:
                        IsOozeDanceEligible = data ? true : false;
                        break;
                    case DATA_NAUSEA_ACHIEVEMENT:
                        IsNauseaEligible = data ? true : false;
                        break;
                    case DATA_ORB_WHISPERER_ACHIEVEMENT:
                        IsOrbWhispererEligible = data ? true : false;
                        break;
                    case DATA_SINDRAGOSA_FROSTWYRMS:
                    {
                        if(FrostwyrmCount == 255)
                            return;

                        if(instance->IsHeroic() && !HeroicAttempts)
                            return;

                        if(GetBossState(DATA_SINDRAGOSA) == DONE)
                            return;

                        switch(data)
                        {
                            case 0:
                                if(FrostwyrmCount)
                                {
                                    --FrostwyrmCount;
                                    if(!FrostwyrmCount)
                                    {
                                        instance->LoadGrid(SindragosaSpawnPos.GetPositionX(), SindragosaSpawnPos.GetPositionY());
                                        if(Creature* boss = instance->SummonCreature(NPC_SINDRAGOSA, SindragosaSpawnPos))
                                            boss->AI()->DoAction(ACTION_START_FROSTWYRM);
                                    }
                                }
                                break;
                            case 1:
                                ++FrostwyrmCount;
                                break;
                            default:
                                FrostwyrmCount = data;
                                break;
                        }
                        break;
                    }
                    case DATA_SPINESTALKER:
                    {
                        if(SpinestalkerTrashCount == 255)
                            return;

                        switch(data)
                        {
                            case 0:
                                if(SpinestalkerTrashCount)
                                {
                                    --SpinestalkerTrashCount;
                                    if(!SpinestalkerTrashCount)
                                        if(Creature* spinestalk = instance->GetCreature(SpinestalkerGUID))
                                            spinestalk->AI()->DoAction(ACTION_START_FROSTWYRM);
                                }
                                break;
                            case 1:
                                ++SpinestalkerTrashCount;
                                break;
                            default:
                                SpinestalkerTrashCount = data;
                                break;
                        }
                        break;
                    }
                    case DATA_RIMEFANG:
                    {
                        if(RimefangTrashCount == 255)
                            return;

                        switch(data)
                        {
                            case 0:
                                if(RimefangTrashCount)
                                {
                                    --RimefangTrashCount;
                                    if(!RimefangTrashCount)
                                        if(Creature* rime = instance->GetCreature(RimefangGUID))
                                            rime->AI()->DoAction(ACTION_START_FROSTWYRM);
                                }
                                break;
                            case 1:
                                ++RimefangTrashCount;
                                break;
                            default:
                                RimefangTrashCount = data;
                                break;
                        }
                        break;
                    }
                    case DATA_COLDFLAME_JETS:
                        ColdflameJetsState = data;
                        if(ColdflameJetsState == DONE)
                        {
                            SaveToDB();
                            for(std::set<uint64>::iterator itr = ColdflameJetGUIDs.begin(); itr != ColdflameJetGUIDs.end(); ++itr)
                                if(Creature* trap = instance->GetCreature(*itr))
                                    trap->AI()->DoAction(ACTION_STOP_TRAPS);
                        }
                        break;
                    case DATA_BLOOD_QUICKENING_STATE:
                    {
                        // skip if nothing changes
                        if(BloodQuickeningState == data)
                            break;
                        // 5 is the index of Blood Quickening
                        if(!sPoolMgr->IsSpawnedObject<Quest>(WeeklyQuestData[5].questId[instance->GetSpawnMode() & 1]))
                            break;
                        switch(data)
                        {
                            case IN_PROGRESS:
                                BloodQuickeningTimer = 60000;
                                BloodQuickeningMinutes = 30;
                                DoUpdateWorldState(WORLDSTATE_SHOW_TIMER, 1);
                                DoUpdateWorldState(WORLDSTATE_EXECUTION_TIME, BloodQuickeningMinutes);
                                break;
                            case DONE:
                                BloodQuickeningTimer = 0;
                                BloodQuickeningMinutes = 0;
                                DoUpdateWorldState(WORLDSTATE_SHOW_TIMER, 0);
                                break;
                            default:
                                break;
                        }

                        BloodQuickeningState = data;
                        SaveToDB();
                        break;
                    }
                    case DATA_ZONE_BUFF_STATUS:
                        ZoneBuffState = data;
                        break;
                    case DATA_SINDRAGOSAS_GAUNTLET:
                        SindragosaGauntlet = data;
                        if(SindragosaGauntlet == DONE)
                        {
                            SaveToDB();
                            if(GameObject* go = instance->GetGameObject(SindragosaGauntletDoorGUID))
                                go->SetGoState(GO_STATE_ACTIVE);
                        }
                        break;
                    default:
                        break;
                }
            }

            bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target*/, uint32 /*miscvalue1*/)
            {
                switch(criteria_id)
                {
                    case CRITERIA_WAITING_A_LONG_TIME_10N:
                    case CRITERIA_WAITING_A_LONG_TIME_10H:
                    case CRITERIA_WAITING_A_LONG_TIME_25N:
                    case CRITERIA_WAITING_A_LONG_TIME_25H:
                        return IsNecroticStack;
                    case CRITERIA_NECK_DEEP_IN_VILE_10N:
                    case CRITERIA_NECK_DEEP_IN_VILE_10H:
                    case CRITERIA_NECK_DEEP_IN_VILE_25N:
                    case CRITERIA_NECK_DEEP_IN_VILE_25H:
                        return IsNeckDeep;
                    case CRITERIA_BONED_10N:
                    case CRITERIA_BONED_25N:
                    case CRITERIA_BONED_10H:
                    case CRITERIA_BONED_25H:
                        return IsBonedEligible;
                    case CRITERIA_DANCES_WITH_OOZES_10N:
                    case CRITERIA_DANCES_WITH_OOZES_25N:
                    case CRITERIA_DANCES_WITH_OOZES_10H:
                    case CRITERIA_DANCES_WITH_OOZES_25H:
                        return IsOozeDanceEligible;
                    case CRITERIA_NAUSEA_10N:
                    case CRITERIA_NAUSEA_25N:
                    case CRITERIA_NAUSEA_10H:
                    case CRITERIA_NAUSEA_25H:
                        return IsNauseaEligible;
                    case CRITERIA_ORB_WHISPERER_10N:
                    case CRITERIA_ORB_WHISPERER_25N:
                    case CRITERIA_ORB_WHISPERER_10H:
                    case CRITERIA_ORB_WHISPERER_25H:
                        return IsOrbWhispererEligible;
                    // Only one criteria for both modes, need to do it like this
                    case CRITERIA_KILL_LANA_THEL_10M:
                    case CRITERIA_ONCE_BITTEN_TWICE_SHY_10N:
                    case CRITERIA_ONCE_BITTEN_TWICE_SHY_10V:
                        return instance->ToInstanceMap()->GetMaxPlayers() == 10;
                    case CRITERIA_KILL_LANA_THEL_25M:
                    case CRITERIA_ONCE_BITTEN_TWICE_SHY_25N:
                    case CRITERIA_ONCE_BITTEN_TWICE_SHY_25V:
                        return instance->ToInstanceMap()->GetMaxPlayers() == 25;
                    default:
                        break;
                }

                return false;
            }

            bool CheckRequiredBosses(uint32 bossId, Player const* player = NULL) const
            {
                if(player && player->isGameMaster())
                    return true;

                switch(bossId)
                {
                    case DATA_THE_LICH_KING:
                        if(!CheckPlagueworks(bossId))
                            return false;
                        if(!CheckCrimsonHalls(bossId))
                            return false;
                        if(!CheckFrostwingHalls(bossId))
                            return false;
                        break;
                    case DATA_SINDRAGOSA:
                    case DATA_VALITHRIA_DREAMWALKER:
                        if(!CheckFrostwingHalls(bossId))
                            return false;
                        break;
                    case DATA_BLOOD_QUEEN_LANA_THEL:
                    case DATA_BLOOD_PRINCE_COUNCIL:
                        if(!CheckCrimsonHalls(bossId))
                            return false;
                        break;
                    case DATA_FESTERGUT:
                    case DATA_ROTFACE:
                    case DATA_PROFESSOR_PUTRICIDE:
                        if(!CheckPlagueworks(bossId))
                            return false;
                        break;
                    default:
                        break;
                }

                if(!CheckLowerSpire(bossId))
                    return false;

                return true;
            }

            bool CheckPlagueworks(uint32 bossId) const
            {
                switch(bossId)
                {
                    case DATA_THE_LICH_KING:
                        if(GetBossState(DATA_PROFESSOR_PUTRICIDE) != DONE)
                            return false;
                        // no break
                    case DATA_PROFESSOR_PUTRICIDE:
                        if(GetBossState(DATA_FESTERGUT) != DONE || GetBossState(DATA_ROTFACE) != DONE)
                            return false;
                        break;
                    default:
                        break;
                }

                return true;
            }

            bool CheckCrimsonHalls(uint32 bossId) const
            {
                switch(bossId)
                {
                    case DATA_THE_LICH_KING:
                        if(GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) != DONE)
                            return false;
                        // no break
                    case DATA_BLOOD_QUEEN_LANA_THEL:
                        if(GetBossState(DATA_BLOOD_PRINCE_COUNCIL) != DONE)
                            return false;
                        break;
                    default:
                        break;
                }

                return true;
            }

            bool CheckFrostwingHalls(uint32 bossId) const
            {
                switch(bossId)
                {
                    case DATA_THE_LICH_KING:
                        if(GetBossState(DATA_SINDRAGOSA) != DONE)
                            return false;
                        // no break
                    case DATA_SINDRAGOSA:
                        if(GetBossState(DATA_VALITHRIA_DREAMWALKER) != DONE)
                            return false;
                        break;
                    default:
                        break;
                }

                return true;
            }

            bool CheckLowerSpire(uint32 bossId) const
            {
                switch(bossId)
                {
                    case DATA_THE_LICH_KING:
                    case DATA_SINDRAGOSA:
                    case DATA_BLOOD_QUEEN_LANA_THEL:
                    case DATA_PROFESSOR_PUTRICIDE:
                    case DATA_VALITHRIA_DREAMWALKER:
                    case DATA_BLOOD_PRINCE_COUNCIL:
                    case DATA_ROTFACE:
                    case DATA_FESTERGUT:
                        if(GetBossState(DATA_DEATHBRINGER_SAURFANG) != DONE)
                            return false;
                        // no break
                    case DATA_DEATHBRINGER_SAURFANG:
                        if(GetBossState(DATA_GUNSHIP_EVENT) != DONE)
                            return false;
                        // no break
                    case DATA_GUNSHIP_EVENT:
                        if(GetBossState(DATA_LADY_DEATHWHISPER) != DONE)
                            return false;
                        // no break
                    case DATA_LADY_DEATHWHISPER:
                        if(GetBossState(DATA_LORD_MARROWGAR) != DONE)
                            return false;
                        // no break
                    case DATA_LORD_MARROWGAR:
                    default:
                        break;
                }

                return true;
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "I C " << GetBossSaveData() << HeroicAttempts << " "
                    << ColdflameJetsState << " " << BloodQuickeningState << " " << BloodQuickeningMinutes;

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(const char* str)
            {
                if(!str)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(str);

                char dataHead1, dataHead2;

                std::istringstream loadStream(str);
                loadStream >> dataHead1 >> dataHead2;

                if(dataHead1 == 'I' && dataHead2 == 'C')
                {
                    for(uint32 i = 0; i < EncounterCount; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if(tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }

                    loadStream >> HeroicAttempts;

                    uint32 temp = 0;
                    loadStream >> temp;
                    ColdflameJetsState = temp ? DONE : NOT_STARTED;

                    loadStream >> temp;
                    BloodQuickeningState = temp ? DONE : NOT_STARTED; // DONE means finished (not success/fail)
                    loadStream >> BloodQuickeningMinutes;
                    loadStream >> temp;
                    SindragosaGauntlet = temp ? DONE: NOT_STARTED;
                    loadStream >> SindragosaGauntlet;
                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            void Update(uint32 diff)
            {
                if(BloodQuickeningState == IN_PROGRESS)
                {
                    if(BloodQuickeningTimer <= diff)
                    {
                        --BloodQuickeningMinutes;
                        BloodQuickeningTimer = 60000;
                        if(BloodQuickeningMinutes)
                        {
                            DoUpdateWorldState(WORLDSTATE_SHOW_TIMER, 1);
                            DoUpdateWorldState(WORLDSTATE_EXECUTION_TIME, BloodQuickeningMinutes);
                        }
                        else
                        {
                            BloodQuickeningState = DONE;
                            DoUpdateWorldState(WORLDSTATE_SHOW_TIMER, 0);
                            if(Creature* bq = instance->GetCreature(BloodQueenLanaThelGUID))
                                bq->AI()->DoAction(ACTION_KILL_MINCHAR);
                        }
                        SaveToDB();
                    }
                    else
                        BloodQuickeningTimer -= diff;
                }
            }

            void PrepareGunshipEvent()
            {
                if(isPrepared || GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                    return;

                if(TeamInInstance == ALLIANCE)
                {
                    if(Transport* th = sMapMgr->LoadTransportInMap(instance, GO_ORGRIM_S_HAMMER_ALLIANCE_ICC, 108000))
                    {
                        th->AddNPCPassengerInInstance(NPC_GB_ORGRIMS_HAMMER, 1.845810f, 1.268872f, 34.526218f, 1.5890f);
                        th->AddNPCPassengerInInstance(NPC_GB_HIGH_OVERLORD_SAURFANG, 37.18615f, 0.00016f, 36.78849f, 3.13683f);
                        th->AddNPCPassengerInInstance(NPC_GB_INVISIBLE_STALKER, 37.18615f, 0.00016f, 36.78849f, 3.13683f);
                        th->AddNPCPassengerInInstance(NPC_GB_KORKRON_BATTLE_MAGE, 47.2929f, -4.308941f, 37.5555f, 3.05033f);
                        th->AddNPCPassengerInInstance(NPC_GB_KORKRON_BATTLE_MAGE, 47.34621f, 4.032004f, 37.70952f, 3.05033f);
                        th->AddNPCPassengerInInstance(NPC_GB_KORKRON_BATTLE_MAGE, 15.03016f, 0.00016f, 37.70952f, 1.55138f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -13.19547f, -27.160213f, 35.47252f, 3.10672f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.33902f, -25.230491f, 33.04052f, 3.00672f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -60.1251f, -1.27014f, 42.8335f, 5.16073f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -48.2651f, 16.78034f, 34.2515f, 0.04292f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -14.8356f, 27.931688f, 33.363f, 1.73231f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 10.2702f, 20.62966f, 35.37483f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 39.32459f, 14.50176f, 36.88428f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 46.17223f, -6.638763f, 37.35444f, 1.32f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 27.4456f, -13.397498f, 36.34746f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 18.16184f, 1.37897f, 35.31705f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.11516f, -0.196236f, 45.15709f, 2.9f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.11844f, -0.19624f, 49.18192f, 1.6f);

                        if(instance->ToInstanceMap()->GetMaxPlayers() == 10)
                        {
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -3.170555f, 28.30652f, 34.21082f, 1.66527f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -12.0928f, 27.65942f, 33.58557f, 1.66527f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 14.92804f, 26.18018f, 35.47803f, 1.66527f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 24.70331f, 25.36584f, 35.97845f, 1.66527f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, -11.44849f, -25.71838f, 33.64343f, 1.49248f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, 12.30336f, -25.69653f, 35.32373f, 1.49248f);
                        } else {
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -3.170555f, 28.30652f, 34.21082f, 1.66527f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -12.0928f, 27.65942f, 33.58557f, 1.66527f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 14.92804f, 26.18018f, 35.47803f, 1.66527f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 24.70331f, 25.36584f, 35.97845f, 1.66527f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 19.92804f, 27.18018f, 35.47803f, 1.66527f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -7.70331f, 28.36584f, 33.88557f, 1.66527f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, -11.44849f, -25.71838f, 33.64343f, 1.49248f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, 12.30336f, -25.69653f, 35.32373f, 1.49248f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, -3.44849f, -25.71838f, 34.21082f, 1.49248f);
                            th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, 3.30336f, -25.69653f, 35.32373f, 1.49248f);
                        }
                    }

                    if(Transport* t = sMapMgr->LoadTransportInMap(instance, GO_THE_SKYBREAKER_ALLIANCE_ICC, 108000))
                    {
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER, -17.156807f, -1.633260f, 20.81273f, 4.52672f);
                        t->AddNPCPassengerInInstance(NPC_GB_MURADIN_BRONZEBEARD, 13.51547f, -0.160213f, 20.87252f, 3.10672f);
                        t->AddNPCPassengerInInstance(NPC_GB_HIHG_CAPTAIN_JUSTIN_BARTLETT, 42.78902f, -0.010491f, 25.24052f, 3.00672f);
                        t->AddNPCPassengerInInstance(NPC_GB_HIGH_OVERLORD_SAURFANG_NOT_VISUAL, -12.9806f, -22.9462f, 21.659f, 4.72416f);
                        t->AddNPCPassengerInInstance(NPC_GB_ZAFOD_BOOMBOX, 18.8042f, 9.907914f, 20.33559f, 3.10672f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_DECKHAND, -64.8423f, 4.4658f, 23.4352f, 2.698897f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_DECKHAND, 35.54972f, 19.93269f, 25.0333f, 4.71242f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_DECKHAND, -36.39837f, 3.13127f, 20.4496f, 1.5708f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_DECKHAND, -36.23974f, -2.75767f, 20.4506f, 4.69496f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_DECKHAND, 41.94677f, 44.08411f, 24.66587f, 1.62032f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 13.51547f, -0.160213f, 20.87252f, 3.10672f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 42.78902f, -0.010491f, 25.24052f, 3.00672f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 14.0551f, 3.65014f, 20.7935f, 3.16073f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 14.0551f, -4.65034f, 20.7915f, 3.04292f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -17.8356f, 0.031688f, 20.823f, 4.73231f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -34.2702f, -26.18966f, 21.37483f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -11.64459f, -19.85176f, 20.88428f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -19.88223f, -6.578763f, 20.57444f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -41.4456f, -7.647498f, 20.49746f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 0.554884f, -1.232897f, 20.53705f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -50.16516f, 9.716236f, 23.58709f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 11.45844f, 16.36624f, 20.54192f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 19.72286f, -2.193787f, 33.06982f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 19.72286f, -2.193787f, 33.06982f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 8.599396f, -28.55855f, 24.79919f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 38.94339f, -33.808f,  25.39618f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 58.15474f, 0.748094f, 41.87663f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 5.607554f, -6.350654f, 34.00357f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 4.780305f, -29.05227f, 35.09634f, 1.6f);

                        if(instance->ToInstanceMap()->GetMaxPlayers() == 10)
                        {
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -5.15231f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -28.0876f, -22.9462f, 21.659f, 4.72416f);
                        } else {
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -5.15231f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -14.9806f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -21.7406f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -28.0876f, -22.9462f, 21.659f, 4.72416f);
                        }
                    }
                }

                if(TeamInInstance == HORDE)
                {
                    if(Transport* t = sMapMgr->LoadTransportInMap(instance, GO_THE_SKYBREAKER_HORDE_ICC, 77800))
                    {
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER, -17.156807f, -1.633260f, 20.81273f, 4.52672f);
                        t->AddNPCPassengerInInstance(NPC_GB_MURADIN_BRONZEBEARD, 13.51547f, -0.160213f, 20.87252f, 3.10672f);
                        t->AddNPCPassengerInInstance(NPC_GB_HIHG_CAPTAIN_JUSTIN_BARTLETT, 42.78902f, -0.010491f, 25.24052f, 3.00672f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_SORCERERS, 14.0551f, 3.65014f, 20.7935f, 3.16073f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_SORCERERS, 14.0551f, -4.65034f, 20.7915f, 3.04292f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_SORCERERS, -17.8356f, 0.031688f, 20.823f, 4.73231f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 13.51547f, -0.160213f, 20.87252f, 3.10672f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 42.78902f, -0.010491f, 25.24052f, 3.00672f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 14.0551f, 3.65014f, 20.7935f, 3.16073f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 14.0551f, -4.65034f, 20.7915f, 3.04292f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -17.8356f, 0.031688f, 20.823f, 4.73231f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -34.2702f, -26.18966f, 21.37483f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -11.64459f, -19.85176f, 20.88428f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -19.88223f, -6.578763f, 20.57444f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -41.4456f, -7.647498f, 20.49746f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 0.554884f, -1.232897f, 20.53705f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -50.16516f, 9.716236f, 23.58709f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 11.45844f, 16.36624f, 20.54192f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 19.72286f, -2.193787f, 33.06982f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 19.72286f, -2.193787f, 33.06982f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 8.599396f, -28.55855f, 24.79919f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 38.94339f, -33.808f,  25.39618f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 58.15474f, 0.748094f, 41.87663f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 5.607554f, -6.350654f, 34.00357f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 4.780305f, -29.05227f, 35.09634f, 1.6f);

                        if(instance->ToInstanceMap()->GetMaxPlayers() == 10)
                        {
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -5.15231f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -14.9806f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -21.7406f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -28.0876f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -8.61003f, 15.483f, 20.4158f, 4.69854f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -27.9583f, 14.8875f, 20.4428f, 4.77865f);
                        } else {
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, 0.15231f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -5.15231f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -14.9806f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -21.7406f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -28.0876f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -33.0876f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -8.61003f, 15.483f, 20.4158f, 4.69854f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -27.9583f, 14.8875f, 20.4428f, 4.77865f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -15.61003f, 15.483f, 20.4158f, 4.69854f);
                            t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -20.9583f, 14.8875f, 20.4428f, 4.77865f);
                        }
                    }

                    if(Transport* th = sMapMgr->LoadTransportInMap(instance,GO_ORGRIM_S_HAMMER_HORDE_ICC, 77800))
                    {
                        th->AddNPCPassengerInInstance(NPC_GB_ORGRIMS_HAMMER, 1.845810f, 1.268872f, 34.526218f, 1.5890f);
                        th->AddNPCPassengerInInstance(NPC_GB_HIGH_OVERLORD_SAURFANG, 37.18615f, 0.00016f, 36.78849f, 3.13683f);
                        th->AddNPCPassengerInInstance(NPC_GB_MURADIN_BRONZEBEARD_NOT_VISUAL, -7.09684f, 30.582f, 34.5013f, 1.53591f);
                        th->AddNPCPassengerInInstance(NPC_GB_INVISIBLE_STALKER, 37.30764f, -0.143823f, 36.7936f, 3.13683f);
                        th->AddNPCPassengerInInstance(NPC_GB_ZAFOD_BOOMBOX, 35.18615f, 15.30652f, 37.64343f, 3.05033f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -13.19547f, -27.160213f, 35.47252f, 3.10672f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.33902f, -25.230491f, 33.04052f, 3.00672f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -60.1251f, -1.27014f, 42.8335f, 5.16073f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -48.2651f, 16.78034f, 34.2515f, 0.04292f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -14.8356f, 27.931688f, 33.363f, 1.73231f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 10.2702f, 20.62966f, 35.37483f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 39.32459f, 14.50176f, 36.88428f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 46.17223f, -6.638763f, 37.35444f, 1.32f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 27.4456f, -13.397498f, 36.34746f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 18.16184f, 1.37897f, 35.31705f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.11516f, -0.196236f, 45.15709f, 2.9f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.11844f, -0.19624f, 49.18192f, 1.6f);

                        if(instance->ToInstanceMap()->GetMaxPlayers() == 10)
                        {
                            th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, 22.6225f, 28.9309f, 36.3929f, 1.53591f);
                            th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, -21.7509f, 29.4207f, 34.2588f, 1.53591f);
                        } else {
                            th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, 22.6225f, 28.9309f, 36.3929f, 1.53591f);
                            th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, 9.87745f, 30.5047f, 35.7147f, 1.53591f);
                            th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, -7.09684f, 30.582f, 34.5013f, 1.53591f);
                            th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, -21.7509f, 29.4207f, 34.2588f, 1.53591f);
                        }
                    }
                }
                isPrepared = true;
            }

        protected:
            std::set<uint64> ColdflameJetGUIDs;
            uint64 LadyDeathwisperElevatorGUID;
            // Gunship battle
            uint32 FirstSquadState;
            uint32 SecondSquadState;
            uint64 SkybreakerBossGUID;
            uint64 OrgrimmarBossGUID;
            uint64 DeathbringerSaurfangGbGUID;
            uint64 MuradinBronzebeardGbGUID;
            uint64 DeathbringerSaurfangNotVisualGUID;
            uint64 MuradinBronzebeardNotVisualGUID;
            uint64 GbBattleMageGUID;
            uint64 DeathbringerSaurfangGUID;
            uint64 DeathbringerSaurfangDoorGUID;
            uint64 DeathbringerSaurfangEventGUID; // Muradin Bronzebeard or High Overlord Saurfang
            uint64 DeathbringersCacheGUID;
            uint64 SaurfangTeleportGUID;
            uint64 PlagueSigilGUID;
            uint64 BloodwingDoor;
            uint64 FrostwingDoor;
            uint64 BloodwingSigilGUID;
            uint64 FrostwingSigilGUID;
            uint64 PutricidePipeGUIDs[2];
            uint64 PutricideGateGUIDs[2];
            uint64 PutricideCollisionGUID;
            uint64 FestergutGUID;
            uint64 RotfaceGUID;
            uint64 ProfessorPutricideGUID;
            uint64 PutricideTableGUID;
            uint64 BloodCouncilGUIDs[3];
            uint64 BloodCouncilControllerGUID;
            uint64 BloodQueenLanaThelGUID;
            uint64 CrokScourgebaneGUID;
            uint64 CrokCaptainGUIDs[4];
            uint64 SisterSvalnaGUID;
            uint64 ValithriaDreamwalkerGUID;
            uint64 ValithriaLichKingGUID;
            uint64 ValithriaTriggerGUID;
            uint64 SindragosaGUID;
            uint64 SpinestalkerGUID;
            uint64 RimefangGUID;
            uint64 uiDreamwalkerCache;
            uint64 TheLichKingGUID;
            uint64 uiTirion;
            uint64 uiTerenasFighter;
            uint64 uiSpiritWarden;
            uint64 uiIceShard1;
            uint64 uiIceShard2;
            uint64 uiIceShard3;
            uint64 uiIceShard4;
            uint64 uiFrostyEdgeInner;
            uint64 uiFrostyEdgeOuter;
            uint64 uiEdgeDestroyWarning;
            uint64 uilavaman;
            uint64 uihangingman;
            uint64 GunShipControllerGUID;
            uint64 GBMuradinGUID;
            uint64 GBSaurfangGUID;
            uint64 GBSkybreakerGUID;
            uint64 GBOgrimsHammerGUID;
            uint64 SindragosasWardGUID;
            uint64 SindragosaGauntletDoorGUID;
            bool  IsNeckDeep;
            bool  IsNecroticStack;
            uint32 SpireSquadState;
            uint32 TeamInInstance;
            uint32 BloodQuickeningTimer;
            uint32 ColdflameJetsState;
            uint32 FrostwyrmCount;
            uint32 SpinestalkerTrashCount;
            uint32 RimefangTrashCount;
            uint32 BloodQuickeningState;
            uint32 HeroicAttempts;
            uint32 SindragosaGauntlet;
            uint16 BloodQuickeningMinutes;
            bool IsBonedEligible;
            bool IsOozeDanceEligible;
            bool IsNauseaEligible;
            bool IsOrbWhispererEligible;
            bool ZoneBuffState;
            bool isPrepared;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_icecrown_citadel_InstanceMapScript(map);
        }
};

void AddSC_instance_icecrown_citadel()
{
    new instance_icecrown_citadel;
}