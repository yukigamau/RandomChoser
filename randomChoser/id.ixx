module;
export module id;

export namespace chooseID
{
	constexpr auto
		idc_stc_title{ 1501 },
		idc_stc_settingBtn{ 1502 },
		// 实际上这个控件用的是IDCLOSE
		// idc_stc_closeBtn{ 1503 },
		idc_stc_chooseBtn{ 1504 },
		idt_scroll{ 1 },
		idt_wait{ 2 };
}

/*
* listModify页面
*/
export namespace listModifyID
{
	constexpr auto
		idc_btn_backSettingPage{ 1501 },
		idc_btn_ifDefaultYes{ 1502 },
		idc_btn_ifDefaultNo{ 1503 },
		idc_btn_save{ 1504 },
		idc_edit_listName{ 1551 },
		idc_edit_writeName{ 1552 },
		idc_static_red{ 1601 };
}

export namespace passwordID
{
	constexpr auto
		idc_btn_backListModify { 1501 },
		idc_btn_needPasswordNo{ 1502 },
		idc_btn_needPasswordYes{ 1503 },
		idc_btn_yes{ 1504 },
		idc_edit_password{ 1551 },
		idc_edit_passwordRe{ 1552 },
		idc_static_ifNeedPassword{ 1601 },
		idc_stc_password{ 1602 },
		idc_stc_passwordRe{ 1603 },
		idc_static_passwordTip{ 1604 };
}

export namespace settingID
{
	constexpr auto
		idc_btn_edit_list{ 1501 },
		idc_btn_open_source_site{ 1502 },
		idc_btn_write_list{ 1505 },
		idc_ccb_default_list{ 1551 };
}