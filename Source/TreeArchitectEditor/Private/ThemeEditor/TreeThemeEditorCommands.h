//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once

/**
* Holds the UI commands for the landscape theme editor
*/
class FTreeThemeEditorCommands
	: public TCommands<FTreeThemeEditorCommands>
{
public:

	/**
	* Default constructor.
	*/
	FTreeThemeEditorCommands();

public:

	// TCommands interface

	virtual void RegisterCommands() override;

public:

	/** Toggles the red channel */
	TSharedPtr<FUICommandInfo> Rebuild;

};

