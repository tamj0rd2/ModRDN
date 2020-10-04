/////////////////////////////////////////////////////////////////////
// File    : RDNDllScore.cpp
// Desc    :
// Created : Sunday, August 26, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNDllScore.h"

#include "UI/RDNText.h"
#include "Stats/RDNStats.h"

#include <ModInterface/DllInterface.h>

#include <Localizer/Localizer.h>

#include <Lua/LuaConfig.h>
#include <Lua/LuaBinding.h>

/////////////////////////////////////////////////////////////////////
// RDNDllScore

namespace
{
	class RDNDllScore : public DLLScoreInterface
	{
		// construction
	public:
		RDNDllScore()
		{
		}

		~RDNDllScore()
		{
		}

		// inherited
	public:
		virtual int TotalDuration() const
		{
			return RDNStats::Instance()->TotalDuration();
		}

		virtual DLLScoreInterface::PlayerState PlayerFinalState(unsigned long idplayer) const
		{
			return RDNStats::Instance()->PlayerFinalState(idplayer);
		}

		virtual const wchar_t *PlayerName(unsigned long idplayer) const
		{
			return RDNStats::Instance()->PlayerName(idplayer);
		}

		virtual size_t UnitsTypeCount(unsigned long idplayer) const
		{
			UNREF_P(idplayer);

			return 0;
		}

		virtual long UnitsTypeAt(unsigned long idplayer, size_t index) const
		{
			UNREF_P(idplayer);
			UNREF_P(index);

			return 0;
		}

		virtual int UnitTotal(unsigned long idplayer, long ebpid) const
		{
			UNREF_P(idplayer);
			UNREF_P(ebpid);

			return 0;
		}

		virtual size_t StatListCount() const
		{
			return 0;
		}

		virtual void StatListAt(size_t idx, wchar_t *out, size_t outlen) const
		{
			UNREF_P(idx);

			wcsncpy(out, L"StatListAt", outlen);
		}

		virtual void StatListTooltipAt(size_t idx, wchar_t *out, size_t outlen) const
		{
			UNREF_P(idx);

			wcsncpy(out, L"StatListTooltipAt", outlen);
		}

		virtual TabType StatListTabTypeAt(size_t idx) const
		{
			UNREF_P(idx);

			return TT_ByPlayer;
		}

		virtual ValueType StatListTypeAt(size_t idx) const
		{
			UNREF_P(idx);

			return VT_Number;
		}

		virtual DecoratorFunc StatListDecoratorFuncAt(size_t idx) const
		{
			UNREF_P(idx);

			return DF_None;
		}

		// value for that column on that tab
		virtual void StatListValue(size_t idx, unsigned long idplayer, int &out) const
		{
			UNREF_P(idx);
			UNREF_P(idplayer);

			out = 0;

			return;
		}

		virtual void StatListValue(size_t idx, unsigned long idplayer, long ebpnetid, int &out) const
		{
			UNREF_P(idx);
			UNREF_P(idplayer);
			UNREF_P(ebpnetid);

			out = 0;

			return;
		}

		virtual size_t ScoresTabsQty() const
		{
			return 0;
		}

		virtual void ScoresTabName(size_t tabIndex, wchar_t *out, size_t outlen) const
		{
			// validate parm
			dbAssert(tabIndex < DLLScoreInterface::NUMTABS);

			wcsncpy(out, L"ScoresTabName", outlen);

			return;
		}

		virtual bool ScoresTabColumn(size_t tabIndex, size_t columnIndex) const
		{
			// validate parm
			dbAssert(tabIndex < DLLScoreInterface::NUMTABS);
			dbAssert(columnIndex < DLLScoreInterface::NUMCOLUMNS);

			return false;
		}

		virtual void ScoresTabColumnName(size_t tabIndex, size_t columnIndex, wchar_t *out, size_t outlen) const
		{
			// validate parm
			dbAssert(tabIndex < DLLScoreInterface::NUMTABS);
			dbAssert(columnIndex < DLLScoreInterface::NUMCOLUMNS);

			wcsncpy(out, L"ScoresTabColumnName", outlen);

			return;
		}

		// tooltip string of that column on that tab
		virtual void ScoresTabColumnTooltip(size_t tabIndex, size_t columnIndex, wchar_t *out, size_t outlen) const
		{
			// validate parm
			dbAssert(tabIndex < DLLScoreInterface::NUMTABS);
			dbAssert(columnIndex < DLLScoreInterface::NUMCOLUMNS);

			wcsncpy(out, L"ScoresTabColumnTooltip", outlen);

			return;
		}

		virtual ValueType ScoresTabColumnType(size_t tabIndex, size_t columnIndex) const
		{
			UNREF_P(tabIndex);
			UNREF_P(columnIndex);

			return VT_Number;
		}

		virtual DecoratorFunc ScoresTabColumnDecoratorFunc(size_t tabIndex, size_t columnIndex) const
		{
			UNREF_P(tabIndex);
			UNREF_P(columnIndex);

			return DF_None;
		}

		virtual TabType ScoresTabType(size_t tabIndex) const
		{
			UNREF_P(tabIndex);

			return TT_Custom;
		}

		virtual void ScoresTabColumnValue(
				size_t tabIndex,
				size_t columnIndex,
				unsigned long arg0,
				int &out) const
		{
			UNREF_P(arg0);

			// validate parm
			dbAssert(tabIndex < DLLScoreInterface::NUMTABS);
			dbAssert(columnIndex < DLLScoreInterface::NUMCOLUMNS);

			out = 0;

			return;
		}

		virtual void ScoresTabColumnValue(
				size_t tabIndex,
				size_t columnIndex,
				unsigned long arg0,
				long arg1,
				int &out) const
		{
			UNREF_P(arg0);
			UNREF_P(arg1);

			// validate parm
			dbAssert(tabIndex < DLLScoreInterface::NUMTABS);
			dbAssert(columnIndex < DLLScoreInterface::NUMCOLUMNS);

			// init out parm
			out = 0;

			return;
		}

		virtual void ScoresTabColumnValue(
				size_t tabIndex,
				size_t columnIndex,
				unsigned long idplayer,
				wchar_t *out,
				size_t outlen) const
		{
			// validate parm
			dbAssert(tabIndex < DLLScoreInterface::NUMTABS);
			dbAssert(columnIndex < DLLScoreInterface::NUMCOLUMNS);

			// init out parm
			out[0] = L'\0';

			// this function should never be called
			UNREF_P(idplayer);
			UNREF_P(out);
			UNREF_P(outlen);

			dbBreak();

			return;
		}
	};
} // namespace

/////////////////////////////////////////////////////////////////////
// RDNDllScore

static RDNDllScore *s_pInstance;

DLLScoreInterface *RDNDllScoreCreate()
{
	s_pInstance = new RDNDllScore;

	return s_pInstance;
}

void RDNDllScoreDestroy(DLLScoreInterface *p)
{
	// validate parm
	dbAssert(p != 0);
	dbAssert(p == s_pInstance);

	delete s_pInstance;

	return;
}
