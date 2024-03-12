#include "sslActorStats.h"

#include "Registry/Define/RaceKey.h"
#include "Registry/Define/Sex.h"
#include "Registry/Stats.h"

namespace Papyrus::ActorStats
{
	std::vector<RE::Actor*> GetAllEncounters(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		std::vector<RE::Actor*> ret{};
		Registry::Statistics::StatisticsData::GetSingleton()->ForEachEncounter([&](Registry::Statistics::ActorEncounter& enc) {
			const auto partner = enc.GetPartner(a_actor);
			if (!partner)
				return false;
			const auto actor = RE::TESForm::LookupByID<RE::Actor>(partner->id);
			if (!actor)
				return false;
			ret.push_back(actor);
			return false;
		});
		return ret;
	}

	std::vector<RE::Actor*> GetAllEncounteredVictims(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		std::vector<RE::Actor*> ret{};
		Registry::Statistics::StatisticsData::GetSingleton()->ForEachEncounter([&](Registry::Statistics::ActorEncounter& enc) {
			const auto partner = enc.GetPartner(a_actor);
			if (!partner || enc.GetTimesVictim(partner->id) <= 0)
				return false;
			if (const auto actor = RE::TESForm::LookupByID<RE::Actor>(partner->id))
				ret.push_back(actor);
			return false;
		});
		return ret;
	}

	std::vector<RE::Actor*> GetAllEncounteredAssailants(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		std::vector<RE::Actor*> ret{};
		Registry::Statistics::StatisticsData::GetSingleton()->ForEachEncounter([&](Registry::Statistics::ActorEncounter& enc) {
			const auto partner = enc.GetPartner(a_actor);
			if (!partner || enc.GetTimesAssailant(partner->id) <= 0)
				return false;
			if (const auto actor = RE::TESForm::LookupByID<RE::Actor>(partner->id))
				ret.push_back(actor);
			return false;
		});
		return ret;
	}

	RE::Actor* GetMostRecentEncounter(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int a_encountertype)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return nullptr;
		}
		return Registry::Statistics::StatisticsData::GetSingleton()->GetMostRecentEncounter(a_actor, Registry::Statistics::ActorEncounter::EncounterType(a_encountertype));
	}

	void AddEncounter(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner, int a_encountertype)
	{
		if (!a_actor || !a_partner) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		const auto type = Registry::Statistics::ActorEncounter::EncounterType(a_encountertype);
		Registry::Statistics::StatisticsData::GetSingleton()->AddEncounter(a_actor, a_partner, type);
	}

	float GetLastEncounterTime(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner)
	{
		if (!a_actor || !a_partner) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		const auto enc = Registry::Statistics::StatisticsData::GetSingleton()->GetEncounter(a_actor, a_partner);
		if (!enc)
			return 0.0;
		return enc->GetLastTimeMet();
	}

	int GetTimesMet(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner)
	{
		if (!a_actor || !a_partner) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		const auto enc = Registry::Statistics::StatisticsData::GetSingleton()->GetEncounter(a_actor, a_partner);
		return enc ? enc->GetTimesMet() : 0;
	}

	int GetTimesVictimzed(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_assailant)
	{
		if (!a_actor || !a_assailant) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		const auto enc = Registry::Statistics::StatisticsData::GetSingleton()->GetEncounter(a_actor, a_assailant);
		return enc ? enc->GetTimesVictim(a_actor->formID) : 0;
	}

	int GetTimesAssaulted(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_victim)
	{
		if (!a_actor || !a_victim) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		const auto enc = Registry::Statistics::StatisticsData::GetSingleton()->GetEncounter(a_actor, a_victim);
		return enc ? enc->GetTimesAssailant(a_actor->formID) : 0;
	}

	std::vector<int> GetEncounterTypesCount(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner)
	{
		if (!a_actor || !a_partner) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return { 0, 0, 0 };
		}
		const auto enc = Registry::Statistics::StatisticsData::GetSingleton()->GetEncounter(a_actor, a_partner);
		if (!enc) {
			return { 0, 0, 0 };
		}
		return {
			enc->GetTimesMet(),
			enc->GetTimesVictim(a_actor->formID),
			enc->GetTimesAssailant(a_actor->formID),
		};
	}

	void ResetStatistics(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		Registry::Statistics::StatisticsData::GetSingleton()->DeleteStatistics(a_actor->GetFormID());
	}

	std::vector<RE::BSFixedString> GetEveryStatisticID(RE::StaticFunctionTag*)
	{
		const auto stats = Registry::Statistics::StatisticsData::GetSingleton();
		std::vector<RE::BSFixedString> ret{};
		stats->ForEachStatistic([&](Registry::Statistics::ActorStats& stats) {
      const auto keys = stats.GetEveryCustomID();
			for (auto&& key : keys) {
				if (key == Purity || key == Lewdness || key == Foreplay)
					continue;
				if (std::ranges::contains(ret, key))
					continue;
				ret.push_back(key);
			}
			return false;
		});
		return ret;
	}

	float GetLegacyStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0.0;
		}
		const auto statdata = Registry::Statistics::StatisticsData::GetSingleton();
    const auto stats = statdata->GetStatistics(a_actor);
		switch (LegacyStatistics(id)) {
		case LegacyStatistics::L_Foreplay:
			{
				auto ret = stats.GetCustomFlt(Foreplay);
				return ret ? *ret : 0;
			}
		case LegacyStatistics::XP_Vaginal:
			return stats.GetStatistic(stats.XP_Vaginal);
		case LegacyStatistics::XP_Anal:
			return stats.GetStatistic(stats.XP_Anal);
		case LegacyStatistics::XP_Oral:
			return stats.GetStatistic(stats.XP_Oral);
		case LegacyStatistics::L_Pure:
      {
				auto ret = stats.GetCustomFlt(Purity);
        return ret ? *ret : 0;
			}
		case LegacyStatistics::L_Lewd:
			{
				auto ret = stats.GetCustomFlt(Lewdness);
				return ret ? *ret : 0;
			}
		case LegacyStatistics::Times_Males:
			return static_cast<float>(statdata->GetNumberEncounters(a_actor, [](auto& it) {
				return it.sex == Registry::Sex::Male && it.race == Registry::RaceKey::Human;
			}));
		case LegacyStatistics::Times_Females:
			return static_cast<float>(statdata->GetNumberEncounters(a_actor, [](auto& it) {
				return it.sex == Registry::Sex::Female && it.race == Registry::RaceKey::Human;
			}));
		case LegacyStatistics::Times_Creatures:
			return static_cast<float>(statdata->GetNumberEncounters(a_actor, [](auto& it) {
				return it.race != Registry::RaceKey::Human;
			}));
		case LegacyStatistics::Times_Masturbation:
			return stats.GetStatistic(stats.TimesMasturbated);
		case LegacyStatistics::Times_Aggressor:
			return stats.GetStatistic(stats.TimesDominant);
		case LegacyStatistics::Times_Victim:
			return stats.GetStatistic(stats.TimesSubmissive);
		case LegacyStatistics::SexCount:
			return stats.GetStatistic(stats.TimesTotal);
		case LegacyStatistics::PlayerSex:
			return static_cast<float>(statdata->GetNumberEncounters(a_actor, [](auto& it) {
				return it.id == 0x14;
			}));
		case LegacyStatistics::Sexuality:
			return stats.GetStatistic(stats.Sexuality);
		case LegacyStatistics::TimeSpent:
			return stats.GetStatistic(stats.SecondsInScene);
		case LegacyStatistics::LastSex_RealTime:
			{
				constexpr auto seconds_in_day = 86400.0f;
				const auto timescale = std::max(RE::Calendar::GetSingleton()->GetTimescale(), 1.0f);
				const auto gametime = stats.GetStatistic(stats.LastUpdate_GameTime);
				return (gametime / timescale) * seconds_in_day;
			}
		case LegacyStatistics::LastSex_GameTime:
			return stats.GetStatistic(stats.LastUpdate_GameTime);
		case LegacyStatistics::Times_VaginalCount:
			return stats.GetStatistic(stats.TimesVaginal);
		case LegacyStatistics::Times_AnalCount:
			return stats.GetStatistic(stats.TimesAnal);
		case LegacyStatistics::Times_OralCount:
			return stats.GetStatistic(stats.TimesOral);
		default:
			a_vm->TraceStack(fmt::format("Invalid id {}", id).c_str(), a_stackID);
      return 0.0;
		}
	}

	std::vector<float> GetAllLegycSkills(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		constexpr auto count = static_cast<int>(LegacyStatistics::Total);
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return std::vector<float>(count);
		}
		std::vector<float> ret{};
		ret.reserve(count);
		for (int i = 0; i < count; i++) {
			ret.push_back(GetLegacyStatistic(a_vm, a_stackID, nullptr, a_actor, i));
		}
		return ret;
	}

	void SetLegacyStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id, float a_value)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		const auto statdata = Registry::Statistics::StatisticsData::GetSingleton();
		auto stats = statdata->GetStatistics(a_actor);
		switch (LegacyStatistics(id)) {
		case LegacyStatistics::L_Foreplay:
			stats.SetCustomFlt(Foreplay, a_value);
			break;
		case LegacyStatistics::XP_Vaginal:
			stats.SetStatistic(stats.XP_Vaginal, a_value);
			break;
		case LegacyStatistics::XP_Anal:
			stats.SetStatistic(stats.XP_Anal, a_value);
			break;
		case LegacyStatistics::XP_Oral:
			stats.SetStatistic(stats.XP_Oral, a_value);
			break;
		case LegacyStatistics::L_Pure:
			stats.SetCustomFlt(Purity, a_value);
			break;
		case LegacyStatistics::L_Lewd:
			stats.SetCustomFlt(Lewdness, a_value);
			break;
		case LegacyStatistics::Times_Males:
		case LegacyStatistics::Times_Females:
		case LegacyStatistics::Times_Creatures:
			// Encounter Statistics
			break;
		case LegacyStatistics::Times_Masturbation:
			stats.SetStatistic(stats.TimesMasturbated, a_value);
			break;
		case LegacyStatistics::Times_Aggressor:
			stats.SetStatistic(stats.TimesDominant, a_value);
			break;
		case LegacyStatistics::Times_Victim:
			stats.SetStatistic(stats.TimesSubmissive, a_value);
			break;
		case LegacyStatistics::SexCount:
			stats.SetStatistic(stats.TimesTotal, a_value);
			break;
		case LegacyStatistics::PlayerSex:
			// Encounter Statistic
			break;
		case LegacyStatistics::Sexuality:
			stats.SetStatistic(stats.Sexuality, a_value);
			break;
		case LegacyStatistics::TimeSpent:
			stats.SetStatistic(stats.SecondsInScene, a_value);
			break;
		case LegacyStatistics::LastSex_RealTime:
			// Only supporting GameTime setter
			break;
		case LegacyStatistics::LastSex_GameTime:
			stats.SetStatistic(stats.LastUpdate_GameTime, a_value);
			break;
		case LegacyStatistics::Times_VaginalCount:
			stats.SetStatistic(stats.TimesVaginal, a_value);
			break;
		case LegacyStatistics::Times_AnalCount:
			stats.SetStatistic(stats.TimesAnal, a_value);
			break;
		case LegacyStatistics::Times_OralCount:
			stats.SetStatistic(stats.TimesOral, a_value);
			break;
		default:
			a_vm->TraceStack(fmt::format("Invalid id {}", id).c_str(), a_stackID);
			return;
		}
	}


} // namespace Papyrus::ActorStats
