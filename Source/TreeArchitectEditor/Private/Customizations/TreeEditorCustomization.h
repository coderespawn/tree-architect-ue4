//$ Copyright 2015-2016, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//


#pragma once

#include "IDetailCustomization.h"

class FTreeArchitectCustomization : public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	// End of IDetailCustomization interface

	static TSharedRef<IDetailCustomization> MakeInstance();

	static FReply BuildTree(IDetailLayoutBuilder* DetailBuilder);
};
