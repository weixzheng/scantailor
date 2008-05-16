/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2007-2008  Joseph Artsimovich <joseph_a@mail.ru>

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

#include "Task.h"
#include "Filter.h"
#include "FilterData.h"
#include "DebugImages.h"
#include "OptionsWidget.h"
#include "AutoManualMode.h"
#include "Dependencies.h"
#include "Params.h"
#include "Settings.h"
#include "TaskStatus.h"
#include "ContentBoxFinder.h"
#include "FilterUiInterface.h"
#include "ImageView.h"
#include "ImageTransformation.h"
#include <QObject>
#include <QRectF>
#include <QDebug>

namespace select_content
{

class Task::UiUpdater : public FilterResult
{
public:
	UiUpdater(IntrusivePtr<Filter> const& filter,
		std::auto_ptr<DebugImages> dbg,
		QImage const& image, ImageTransformation const& xform,
		OptionsWidget::UiData const& ui_data);
	
	virtual void updateUI(FilterUiInterface* ui);
	
	virtual IntrusivePtr<AbstractFilter> filter() { return m_ptrFilter; }
private:
	IntrusivePtr<Filter> m_ptrFilter;
	std::auto_ptr<DebugImages> m_ptrDbg;
	QImage m_image;
	ImageTransformation m_xform;
	OptionsWidget::UiData m_uiData;
};


Task::Task(IntrusivePtr<Filter> const& filter,
	IntrusivePtr<Settings> const& settings,
	LogicalPageId const& page_id, bool const debug)
:	m_ptrFilter(filter),
	m_ptrSettings(settings),
	m_pageId(page_id)
{
	if (debug) {
		m_ptrDbg.reset(new DebugImages);
	}
}

Task::~Task()
{
}

FilterResultPtr
Task::process(TaskStatus const& status, FilterData const& data)
{
	status.throwIfCancelled();
	
	Dependencies const deps(data.xform().resultingCropArea());
	
	std::auto_ptr<Params> params(m_ptrSettings->getPageParams(m_pageId));
	if (params.get() && !params->dependencies().matches(deps)) {
		params.reset();
	}
	
	OptionsWidget::UiData ui_data;
	
	if (params.get()) {
		ui_data.setContentRect(params->contentRect());
		ui_data.setDependencies(params->dependencies());
		ui_data.setMode(params->mode());
	} else {
		ui_data.setContentRect(
			ContentBoxFinder::findContentBox(
				status, data, m_ptrDbg.get()
			)
		);
		ui_data.setDependencies(deps);
		ui_data.setMode(MODE_AUTO);
		Params const new_params(ui_data.contentRect(), deps, MODE_AUTO);
		m_ptrSettings->setPageParams(m_pageId, new_params);
	}
	
	status.throwIfCancelled();
	
	return FilterResultPtr(
		new UiUpdater(
			m_ptrFilter, m_ptrDbg, data.image(),
			data.xform(), ui_data
		)
	);
}


/*============================ Task::UiUpdater ==========================*/

Task::UiUpdater::UiUpdater(
	IntrusivePtr<Filter> const& filter,
	std::auto_ptr<DebugImages> dbg,
	QImage const& image, ImageTransformation const& xform,
	OptionsWidget::UiData const& ui_data)
:	m_ptrFilter(filter),
	m_ptrDbg(dbg),
	m_image(image),
	m_xform(xform),
	m_uiData(ui_data)
{
}

void
Task::UiUpdater::updateUI(FilterUiInterface* ui)
{
	// This function is executed from the GUI thread.
	
	OptionsWidget* const opt_widget = m_ptrFilter->optionsWidget();
	opt_widget->postUpdateUI(m_uiData);
	ui->setOptionsWidget(opt_widget);
	
	ImageView* view = new ImageView(m_image, m_xform, m_uiData.contentRect());
	ui->setImageWidget(view, m_ptrDbg.get());
	
	QObject::connect(
		view, SIGNAL(manualContentRectSet(QRectF const&)),
		opt_widget, SLOT(manualContentRectSet(QRectF const&))
	);
}

} // namespace select_content