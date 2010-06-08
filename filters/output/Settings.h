/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OUTPUT_SETTINGS_H_
#define OUTPUT_SETTINGS_H_

#include "RefCountable.h"
#include "NonCopyable.h"
#include "PageId.h"
#include "Dpi.h"
#include "ColorParams.h"
#include "OutputParams.h"
#include "DespeckleLevel.h"
#include "ZoneSet.h"
#include "PropertySet.h"
#include <QMutex>
#include <map>
#include <memory>

namespace output
{

class Params;

class Settings : public RefCountable
{
	DECLARE_NON_COPYABLE(Settings)
public:
	Settings();
	
	virtual ~Settings();
	
	void clear();
	
	Params getParams(PageId const& page_id) const;
	
	void setParams(PageId const& page_id, Params const& params);
	
	ColorParams getColorParams(PageId const& page_id) const;
	
	void setColorParams(PageId const& page_id, ColorParams const& params);
	
	void setColorParamsForAllPages(ColorParams const& params);
	
	Dpi getDpi(PageId const& page_id) const;
	
	void setDpi(PageId const& page_id, Dpi const& dpi);
	
	void setDpiForAllPages(Dpi const& dpi);

	DespeckleLevel getDespeckleLevel(PageId const& page_id) const;

	void setDespeckleLevel(PageId const& page_id, DespeckleLevel level);

	void setDespeckleLevelForAllPages(DespeckleLevel level);
	
	std::auto_ptr<OutputParams> getOutputParams(PageId const& page_id) const;
	
	void removeOutputParams(PageId const& page_id);
	
	void setOutputParams(PageId const& page_id, OutputParams const& params);

	ZoneSet pictureZonesForPage(PageId const& page_id) const;

	ZoneSet fillZonesForPage(PageId const& page_id) const;

	void setPictureZones(PageId const& page_id, ZoneSet const& zones);

	void setFillZones(PageId const& page_id, ZoneSet const& zones);

	/**
	 * For now, default zone properties are not persistent.
	 * They may become persistent later though.
	 */
	PropertySet defaultPictureZoneProperties() const;

	PropertySet defaultFillZoneProperties() const;

	void setDefaultPictureZoneProperties(PropertySet const& props);

	void setDefaultFillZoneProperties(PropertySet const& props);
private:
	typedef std::map<PageId, Dpi> PerPageDpi;
	typedef std::map<PageId, DespeckleLevel> PerPageDespeckleLevel;
	typedef std::map<PageId, ColorParams> PerPageColorParams;
	typedef std::map<PageId, OutputParams> PerPageOutputParams;
	typedef std::map<PageId, ZoneSet> PerPageZones;
	
	Dpi getDpiLocked(PageId const& page_id) const;
	
	void setDpiLocked(PageId const& page_id, Dpi const& dpi);

	DespeckleLevel getDespeckleLevelLocked(PageId const& page_id) const;

	void setDespeckleLevelLocked(PageId const& page_id, DespeckleLevel level);
	
	ColorParams getColorParamsLocked(PageId const& page_id) const;
	
	void setColorParamsLocked(
		PageId const& page_id, ColorParams const& params);
	
	mutable QMutex m_mutex;
	PerPageDpi m_perPageDpi;
	PerPageDespeckleLevel m_perPageDespeckleLevel;
	PerPageColorParams m_perPageColorParams;
	PerPageOutputParams m_perPageOutputParams;
	PerPageZones m_perPagePictureZones;
	PerPageZones m_perPageFillZones;
	Dpi m_defaultDpi;
	ColorParams m_defaultColorParams;
	PropertySet m_defaultPictureZoneProps;
	PropertySet m_defaultFillZoneProps;
	DespeckleLevel m_defaultDespeckleLevel;
};

} // namespace output

#endif
