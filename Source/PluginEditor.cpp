/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


Placeholder::Placeholder() {
    

}
void Placeholder::paint(juce::Graphics& g) {
	using namespace juce;
	auto bounds = getLocalBounds();


	g.setColour(juce::Colour(110, 164, 38));
	g.fillAll();

	auto localbounds = bounds;
	bounds.reduce(3, 3);
	g.setColour(juce::Colour(24, 25, 26));
	g.fillRoundedRectangle(bounds.toFloat(), 3);

	g.drawRect(localbounds);
}

CustomLookAndFeel::CustomLookAndFeel() {

}
void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {

	auto getComponentBound = juce::Rectangle<int>(x, y, width, height).toFloat(); // add .reduced for smaller bounds
	rotaryStartAngle = -juce::MathConstants<float>::pi * 2.0f * 3.0f / 8.0f;				// zacetni kot v radianih
	rotaryEndAngle = juce::MathConstants<float>::pi * 2.0f * 3.0f / 8.0f;				// koncni kot v rad
	auto AngleSizePath = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle); // kot ki ga zajema slider


	// notranji kvadrat za slider

	getComponentBound = getComponentBound.constrainedWithin(juce::Rectangle<int>(x, y, juce::jmin(width, height), juce::jmin(width, height)).toFloat());



	auto lineMarkerFromCenter = getComponentBound.reduced(getComponentBound.getHeight() / 6.5f); // lenght of value marker from center 
	auto lineMarkerFromEdge = getComponentBound.reduced(getComponentBound.getHeight() / 7.4f); // lenght from circle edge
	auto lineValueThickness = getComponentBound.getHeight() / 120.0f;


	auto radius = jmin(width / 2.25, height / 2.25) - 4.5;
	auto getCenterX = x + width * 0.5f;

	getComponentBound.setPosition(0, 0);

	g.setColour(juce::Colour(110, 164, 38));

	juce::Point<float> createCircle(lineMarkerFromEdge.getTopLeft().getX(), getComponentBound.getCentreY()); // scale around to make full circle


	//Create circle, pivot around the centre of rectangle
	//createCircle.applyTransform(juce::AffineTransform::rotation(-2.0f * juce::MathConstants<float>::pi * 70.0f / 360.0f, getComponentBound.getCentreX(), getComponentBound.getCentreY()));
	float startingAngleDrawLineMarker = 40.0f;
	float stepToLeft = -2.0f;
	float stepToRight = 2.0f;
	createCircle.applyTransform(juce::AffineTransform::rotation(stepToLeft * juce::MathConstants<float>::pi * startingAngleDrawLineMarker / 360.0f, 
		getComponentBound.getCentreX(), getComponentBound.getCentreY()));
	
	juce::Line<float> markerLine(getComponentBound.getCentre() , createCircle);
	g.drawLine(markerLine, lineValueThickness);

	for (int numOfLines = 0; numOfLines < 24; numOfLines++)
	{
		//Go around circle, draw line marker for each step
		createCircle.applyTransform(juce::AffineTransform::rotation(stepToRight * juce::MathConstants<float>::pi * 10.0f / 360.0f, getComponentBound.getCentreX(), getComponentBound.getCentreY()));
		markerLine.setEnd(createCircle);
		g.drawLine(markerLine, lineValueThickness);
	}



	g.setColour(juce::Colour(36, 37, 38));
	g.fillEllipse(lineMarkerFromCenter);


	// Slider Dial
	auto arcRadius = lineMarkerFromCenter.getWidth() / stepToRight;

	juce::Point<float> SliderDialEdge(getComponentBound.getCentreX() + arcRadius * std::cos(AngleSizePath - juce::MathConstants<float>::halfPi), 
		getComponentBound.getCentreY() + arcRadius * std::sin(AngleSizePath - juce::MathConstants<float>::halfPi));


	auto startingDialPointX = (getComponentBound.getCentre().getX() + SliderDialEdge.getX()) / 2.0f;
	auto startingDialPointY = (getComponentBound.getCentre().getY() + SliderDialEdge.getY()) / 2.0f;
	juce::Point<float> LinesStartPoint(startingDialPointX, startingDialPointY);

	juce::Line<float> drawDial(LinesStartPoint , SliderDialEdge);

	g.setColour(juce::Colour(110, 164, 38));
	g.drawLine(drawDial, lineValueThickness * 2);

	
}

ReverbControls::ReverbControls(juce::AudioProcessorValueTreeState& apv): apvts(apv) {
	juce::LookAndFeel::setDefaultLookAndFeel(&clnf);

	addAndMakeVisible(sizeSlider);
	addAndMakeVisible(widthSlider);
	addAndMakeVisible(dampSlider);
	addAndMakeVisible(drySlider);
	addAndMakeVisible(wetSlider);
	addAndMakeVisible(offBtn);

	offBtn.setButtonText(juce::String(juce::CharPointer_UTF8("Off")));
	offBtn.setClickingTogglesState(true);
	offBtn.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
	offBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentWhite);
	offBtn.setColour(juce::TextButton::textColourOnId, juce::Colour(110, 164, 38));
	offBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

	sizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	widthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	dampSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	drySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	wetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);

	sizeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	widthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	dampSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	drySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	wetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

	sizeSlider.setNormalisableRange(juce::NormalisableRange<double>(0.0f, 1.0f, 0.001f, 1.0f));
	widthSlider.setNormalisableRange(juce::NormalisableRange<double>(0.0f, 1.0f, 0.001f, 1.0f));
	dampSlider.setNormalisableRange(juce::NormalisableRange<double>(0.0f, 1.0f, 0.001f, 1.0f));
	drySlider.setNormalisableRange(juce::NormalisableRange<double>(0.0f, 1.0f, 0.001f, 1.0f));
	wetSlider.setNormalisableRange(juce::NormalisableRange<double>(0.0f, 1.0f, 0.001f, 1.0f));

	sizeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv,"Size", sizeSlider);
	widthSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Width", widthSlider);
	dampSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Damp", dampSlider);
	drySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Dry", drySlider);
	wetSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Wet", wetSlider);
	offBtnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apv, "reverboff", offBtn);



}
ReverbControls::~ReverbControls() {
	
	
}
void ReverbControls::paint(juce::Graphics& g){
	
}
void ReverbControls::resized() {
	auto bounds = getLocalBounds();
	using namespace juce;


	FlexBox flexbox;
	flexbox.flexDirection = FlexBox::Direction::row;
	flexbox.flexWrap = FlexBox::Wrap::noWrap;
	auto spacer = FlexItem().withWidth(8);

	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(sizeSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(widthSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(dampSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(drySlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(wetSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	 spacer = FlexItem().withHeight(20);

	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(offBtn).withWidth(20).withHeight(15));
	flexbox.items.add(spacer);

	spacer = FlexItem().withWidth(4);

	flexbox.performLayout(bounds);
}
LowBandControls::LowBandControls(juce::AudioProcessorValueTreeState& apv): apvts(apv)
 {
	juce::LookAndFeel::setDefaultLookAndFeel(&clnf);

	attackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	releaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	thresholdSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	ratioSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);

	attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	thresholdSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	ratioSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

	attackSlider.setNormalisableRange(juce::NormalisableRange<double>(5, 500, 1, 1));
	releaseSlider.setNormalisableRange(juce::NormalisableRange<double>(5, 500, 1, 1));
	thresholdSlider.setNormalisableRange(juce::NormalisableRange<double>(-60, 12, 1, 1));
	ratioSlider.setNormalisableRange(juce::NormalisableRange<double>(1, 100, 1, 1));

	addAndMakeVisible(attackSlider);
	addAndMakeVisible(releaseSlider);
	addAndMakeVisible(thresholdSlider);
	addAndMakeVisible(ratioSlider);
	addAndMakeVisible(testLabel);



	testLabel.setFont(juce::Font(10.0f));
	testLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	attackSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Attack Low", attackSlider);
	releaseSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Release Low", releaseSlider);
	thresholdSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Threshold Low", thresholdSlider);
	ratioSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Ratio Low", ratioSlider);



}
LowBandControls::~LowBandControls() {

}
void LowBandControls::paint(Graphics& g) {
	using namespace juce;
	
}
void LowBandControls::resized() {


	testLabel.setBounds(77, 400, 200, 200);
	auto bounds = getLocalBounds();
	using namespace juce;


	FlexBox flexbox;
	flexbox.flexDirection = FlexBox::Direction::row;
	flexbox.flexWrap = FlexBox::Wrap::noWrap;
	auto spacer = FlexItem().withWidth(8);

	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(attackSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(releaseSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(thresholdSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(ratioSlider).withFlex(1.f));
	spacer = FlexItem().withWidth(4);

	flexbox.performLayout(bounds);
}

MidBandControls::MidBandControls(juce::AudioProcessorValueTreeState& apvs): apvts(apvs) {

	juce::LookAndFeel::setDefaultLookAndFeel(&clnf);

	attackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	releaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	thresholdSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	ratioSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);

	attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	thresholdSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	ratioSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

	attackSlider.setNormalisableRange(juce::NormalisableRange<double>(5, 500, 1, 1));
	releaseSlider.setNormalisableRange(juce::NormalisableRange<double>(5, 500, 1, 1));
	thresholdSlider.setNormalisableRange(juce::NormalisableRange<double>(-60, 12, 1, 1));
	ratioSlider.setNormalisableRange(juce::NormalisableRange<double>(1, 100, 1, 1));


	addAndMakeVisible(attackSlider);
	addAndMakeVisible(releaseSlider);
	addAndMakeVisible(thresholdSlider);
	addAndMakeVisible(ratioSlider);

	attackSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvs, "Attack Mid", attackSlider);
	releaseSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvs, "Release Mid", releaseSlider);
	thresholdSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvs, "Threshold Mid", thresholdSlider);
	ratioSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvs, "Ratio Mid", ratioSlider);

}
MidBandControls::~MidBandControls() {

}

void MidBandControls::paint(juce::Graphics& g){}
void MidBandControls::resized() {
	auto bounds = getLocalBounds();
	using namespace juce;


	FlexBox flexbox;
	flexbox.flexDirection = FlexBox::Direction::row;
	flexbox.flexWrap = FlexBox::Wrap::noWrap;
	auto spacer = FlexItem().withWidth(8);

	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(attackSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(releaseSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(thresholdSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(ratioSlider).withFlex(1.f));
	spacer = FlexItem().withWidth(4);

	flexbox.performLayout(bounds);
}
HighBandControls::HighBandControls(juce::AudioProcessorValueTreeState& apv) : apvts(apv)
{
	juce::LookAndFeel::setDefaultLookAndFeel(&clnf);

	attackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	releaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	thresholdSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	ratioSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);

	attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	thresholdSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	ratioSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

	attackSlider.setNormalisableRange(juce::NormalisableRange<double>(5, 500, 1, 1));
	releaseSlider.setNormalisableRange(juce::NormalisableRange<double>(5, 500, 1, 1));
	thresholdSlider.setNormalisableRange(juce::NormalisableRange<double>(-60, 12, 1, 1));
	ratioSlider.setNormalisableRange(juce::NormalisableRange<double>(1, 100, 1, 1));


	addAndMakeVisible(attackSlider);
	addAndMakeVisible(releaseSlider);
	addAndMakeVisible(thresholdSlider);
	addAndMakeVisible(ratioSlider);

	attackSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Attack High", attackSlider);
	releaseSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Release High", releaseSlider);
	thresholdSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Threshold High", thresholdSlider);
	ratioSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "Ratio High", ratioSlider);

	
}
HighBandControls::~HighBandControls() {

}
void HighBandControls::paint(Graphics& g) {

}
void HighBandControls::resized() {

	auto bounds = getLocalBounds();
	using namespace juce;


	FlexBox flexbox;
	flexbox.flexDirection = FlexBox::Direction::row;
	flexbox.flexWrap = FlexBox::Wrap::noWrap;
	auto spacer = FlexItem().withWidth(8);

	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(attackSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(releaseSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(thresholdSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(ratioSlider).withFlex(1.f));
	spacer = FlexItem().withWidth(4);

	flexbox.performLayout(bounds);
}

ThreeButtonsLow::ThreeButtonsLow(juce::AudioProcessorValueTreeState& apvts) {
	//juce::LookAndFeel::setDefaultLookAndFeel(&clnf);


	addAndMakeVisible(offBtn);
	addAndMakeVisible(solobtn);
	addAndMakeVisible(mutebtn);

	offBtn.addListener(this);
	solobtn.addListener(this);
	mutebtn.addListener(this);

	offBtn.setButtonText(juce::String(juce::CharPointer_UTF8("B")));
	offBtn.setClickingTogglesState(true);
	offBtn.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
	offBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentWhite);
	offBtn.setColour(juce::TextButton::textColourOnId, juce::Colour(110, 164, 38));
	offBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

	solobtn.setButtonText(juce::String(juce::CharPointer_UTF8("S")));
	solobtn.setClickingTogglesState(true);
	solobtn.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
	solobtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentWhite);
	solobtn.setColour(juce::TextButton::textColourOnId, juce::Colour(110, 164, 38));
	solobtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
	
	mutebtn.setButtonText(juce::String(juce::CharPointer_UTF8("M")));
	mutebtn.setClickingTogglesState(true);
	mutebtn.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
	mutebtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentWhite);
	mutebtn.setColour(juce::TextButton::textColourOnId, juce::Colour(110, 164, 38));
	mutebtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);


	offBtnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "Comp Off Low", offBtn);
	solobtnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "Solo Low", solobtn);
	mutebtnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "Mute Low", mutebtn);

}
ThreeButtonsLow::~ThreeButtonsLow() {

}

void ThreeButtonsLow::paint(juce::Graphics& g) {};
void ThreeButtonsLow::resized() {
	auto bounds = getLocalBounds();
	using namespace juce;


	FlexBox flexbox;
	flexbox.flexDirection = FlexBox::Direction::column;
	flexbox.flexWrap = FlexBox::Wrap::noWrap;
	auto spacer = FlexItem().withWidth(8);

	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(offBtn).withFlex(1.5f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(solobtn).withFlex(1.5f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(mutebtn).withFlex(1.5f));
	flexbox.items.add(spacer);
	spacer = FlexItem().withWidth(4);

	flexbox.performLayout(bounds);
};


void ThreeButtonsLow::buttonClicked(juce::Button* button) {

}

ThreeButtonsMid::ThreeButtonsMid(juce::AudioProcessorValueTreeState& apvts) {
	//juce::LookAndFeel::setDefaultLookAndFeel(&clnf);


	addAndMakeVisible(offBtn);
	addAndMakeVisible(solobtn);
	addAndMakeVisible(mutebtn);

	offBtn.addListener(this);
	solobtn.addListener(this);
	mutebtn.addListener(this);

	offBtn.setButtonText(juce::String(juce::CharPointer_UTF8("B")));
	offBtn.setClickingTogglesState(true);
	offBtn.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
	offBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentWhite);
	offBtn.setColour(juce::TextButton::textColourOnId, juce::Colour(110, 164, 38));
	offBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

	solobtn.setButtonText(juce::String(juce::CharPointer_UTF8("S")));
	solobtn.setClickingTogglesState(true);
	solobtn.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
	solobtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentWhite);
	solobtn.setColour(juce::TextButton::textColourOnId, juce::Colour(110, 164, 38));
	solobtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

	mutebtn.setButtonText(juce::String(juce::CharPointer_UTF8("M")));
	mutebtn.setClickingTogglesState(true);
	mutebtn.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
	mutebtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentWhite);
	mutebtn.setColour(juce::TextButton::textColourOnId, juce::Colour(110, 164, 38));
	mutebtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);


	offBtnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "Comp Off Mid", offBtn);
	solobtnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "Solo Mid", solobtn);
	mutebtnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "Mute Mid", mutebtn);

}
ThreeButtonsMid::~ThreeButtonsMid() {

}


void ThreeButtonsMid::paint(juce::Graphics& g) {};
void ThreeButtonsMid::resized() {
	auto bounds = getLocalBounds();
	using namespace juce;


	FlexBox flexbox;
	flexbox.flexDirection = FlexBox::Direction::column;
	flexbox.flexWrap = FlexBox::Wrap::noWrap;
	auto spacer = FlexItem().withWidth(8);

	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(offBtn).withFlex(1.5f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(solobtn).withFlex(1.5f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(mutebtn).withFlex(1.5f));
	flexbox.items.add(spacer);
	spacer = FlexItem().withWidth(4);

	flexbox.performLayout(bounds);
};

ThreeButtonsHigh::ThreeButtonsHigh(juce::AudioProcessorValueTreeState& apvts) {
	//juce::LookAndFeel::setDefaultLookAndFeel(&clnf);


	addAndMakeVisible(offBtn);
	addAndMakeVisible(solobtn);
	addAndMakeVisible(mutebtn);

	offBtn.addListener(this);
	solobtn.addListener(this);
	mutebtn.addListener(this);

	offBtn.setButtonText(juce::String(juce::CharPointer_UTF8("B")));
	offBtn.setClickingTogglesState(true);
	offBtn.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
	offBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentWhite);
	offBtn.setColour(juce::TextButton::textColourOnId, juce::Colour(110, 164, 38));
	offBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

	solobtn.setButtonText(juce::String(juce::CharPointer_UTF8("S")));
	solobtn.setClickingTogglesState(true);
	solobtn.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
	solobtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentWhite);
	solobtn.setColour(juce::TextButton::textColourOnId, juce::Colour(110, 164, 38));
	solobtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

	mutebtn.setButtonText(juce::String(juce::CharPointer_UTF8("M")));
	mutebtn.setClickingTogglesState(true);
	mutebtn.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
	mutebtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentWhite);
	mutebtn.setColour(juce::TextButton::textColourOnId, juce::Colour(110, 164, 38));
	mutebtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);


	offBtnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "Comp Off High", offBtn);
	solobtnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "Solo High", solobtn);
	mutebtnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "Mute High", mutebtn);

}
ThreeButtonsHigh::~ThreeButtonsHigh() {

}

void ThreeButtonsHigh::paint(juce::Graphics& g) {};
void ThreeButtonsHigh::resized() {
	auto bounds = getLocalBounds();
	using namespace juce;


	FlexBox flexbox;
	flexbox.flexDirection = FlexBox::Direction::column;
	flexbox.flexWrap = FlexBox::Wrap::noWrap;
	auto spacer = FlexItem().withWidth(8);

	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(offBtn).withFlex(1.5f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(solobtn).withFlex(1.5f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(mutebtn).withFlex(1.5f));
	flexbox.items.add(spacer);
	spacer = FlexItem().withWidth(4);

	flexbox.performLayout(bounds);
};



void TheEqualizerAudioProcessorEditor::InitializeLabelsForCompressor() {


	lowBandTitles.setText("Attack           Release         Threshold				Ratio", juce::dontSendNotification);
	lowBandTitles.setFont(juce::Font(14.0f));
	lowBandTitles.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	lowBandRanges.setText("5ms		1000ms		5ms			1000ms		-60dB		12dB			1:1			1:100", juce::dontSendNotification);
	lowBandRanges.setFont(juce::Font(10.0f));
	lowBandRanges.setColour(juce::Label::textColourId, juce::Colours::lightgreen);



	midBandTitles.setText("Attack           Release         Threshold				Ratio", juce::dontSendNotification);
	midBandTitles.setFont(juce::Font(14.0f));
	midBandTitles.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	midBandRanges.setText("5ms		1000ms		5ms			1000ms		-60dB		12dB			1:1			1:100", juce::dontSendNotification);
	midBandRanges.setFont(juce::Font(10.0f));
	midBandRanges.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	highBandTitles.setText("Attack           Release         Threshold				Ratio", juce::dontSendNotification);
	highBandTitles.setFont(juce::Font(14.0f));
	highBandTitles.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	highBandRanges.setText("5ms		1000ms		5ms			1000ms		-60dB		12dB			1:1			1:100", juce::dontSendNotification);
	highBandRanges.setFont(juce::Font(10.0f));
	highBandRanges.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	reverbTitles.setText("Size				Width			  Damp				Dry				  Wet", juce::dontSendNotification);
	reverbTitles.setFont(juce::Font(14.0f));
	reverbTitles.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	reverbRange.setText("0			 1				 0			  1 			0			  1				0			   1				0			    1", juce::dontSendNotification);
	reverbRange.setFont(juce::Font(10.0f));
	reverbRange.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	distortionTitles.setText("              Gain				      Drive			      Blend		      Output Gain				  ", juce::dontSendNotification);
	distortionTitles.setFont(juce::Font(14.0f));
	distortionTitles.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	distortionRanges.setText("-20dB			 40dB				 0			  20dB 			   0			   1				 -20dB			 40dB				", juce::dontSendNotification);
	distortionRanges.setFont(juce::Font(10.0f));
	distortionRanges.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	LowAttackLabel.setFont(juce::Font(14.0f));
	LowAttackLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	LowReleaseLabel.setFont(juce::Font(14.0f));
	LowReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	LowThresholdLabel.setFont(juce::Font(14.0f));
	LowThresholdLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	LowRatioLabel.setFont(juce::Font(14.0f));
	LowRatioLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	MidAttackLabel.setFont(juce::Font(14.0f));
	MidAttackLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	MidReleaseLabel.setFont(juce::Font(14.0f));
	MidReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	MidThresholdLabel.setFont(juce::Font(14.0f));
	MidThresholdLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	MidRatioLabel.setFont(juce::Font(14.0f));
	MidRatioLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	HighAttackLabel.setFont(juce::Font(14.0f));
	HighAttackLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	HighReleaseLabel.setFont(juce::Font(14.0f));
	HighReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	HighThresholdLabel.setFont(juce::Font(14.0f));
	HighThresholdLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	HighRatioLabel.setFont(juce::Font(14.0f));
	HighRatioLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

	lowBandControls.attackSlider.onValueChange = [this] { LowAttackLabel.setText(String(lowBandControls.attackSlider.getValue()) + " ms", juce::dontSendNotification); };
	lowBandControls.releaseSlider.onValueChange = [this] { LowReleaseLabel.setText(String(lowBandControls.releaseSlider.getValue()) + " ms", juce::dontSendNotification); };
	lowBandControls.thresholdSlider.onValueChange = [this] { LowThresholdLabel.setText(String(lowBandControls.thresholdSlider.getValue()) + " dB", juce::dontSendNotification); };
	lowBandControls.ratioSlider.onValueChange = [this] { LowRatioLabel.setText(String(lowBandControls.ratioSlider.getValue()) + ":1", juce::dontSendNotification); };

	midBandControls.attackSlider.onValueChange = [this] { MidAttackLabel.setText(String(midBandControls.attackSlider.getValue()) + " ms", juce::dontSendNotification); };
	midBandControls.releaseSlider.onValueChange = [this] { MidReleaseLabel.setText(String(midBandControls.releaseSlider.getValue()) + " ms", juce::dontSendNotification); };
	midBandControls.thresholdSlider.onValueChange = [this] { MidThresholdLabel.setText(String(midBandControls.thresholdSlider.getValue()) + " dB", juce::dontSendNotification); };
	midBandControls.ratioSlider.onValueChange = [this] { MidRatioLabel.setText(String(midBandControls.ratioSlider.getValue()) + ":1", juce::dontSendNotification); };


	highBandControls.attackSlider.onValueChange = [this] { HighAttackLabel.setText(String(highBandControls.attackSlider.getValue()) + " ms", juce::dontSendNotification); };
	highBandControls.releaseSlider.onValueChange = [this] { HighReleaseLabel.setText(String(highBandControls.releaseSlider.getValue()) + " ms", juce::dontSendNotification); };
	highBandControls.thresholdSlider.onValueChange = [this] { HighThresholdLabel.setText(String(highBandControls.thresholdSlider.getValue()) + " dB", juce::dontSendNotification); };
	highBandControls.ratioSlider.onValueChange = [this] { HighRatioLabel.setText(String(highBandControls.ratioSlider.getValue()) + ":1", juce::dontSendNotification); };

	LowAttackLabel.setAlwaysOnTop(true);
	LowReleaseLabel.setAlwaysOnTop(true);
	LowThresholdLabel.setAlwaysOnTop(true);
	LowRatioLabel.setAlwaysOnTop(true);


	MidAttackLabel.setAlwaysOnTop(true);
	MidReleaseLabel.setAlwaysOnTop(true);
	MidThresholdLabel.setAlwaysOnTop(true);
	MidRatioLabel.setAlwaysOnTop(true);


	HighAttackLabel.setAlwaysOnTop(true);
	HighReleaseLabel.setAlwaysOnTop(true);
	HighThresholdLabel.setAlwaysOnTop(true);
	HighRatioLabel.setAlwaysOnTop(true);

	addAndMakeVisible(LowReleaseLabel);
	addAndMakeVisible(LowThresholdLabel);
	addAndMakeVisible(LowRatioLabel);
	addAndMakeVisible(LowAttackLabel);


	addAndMakeVisible(MidAttackLabel);
	addAndMakeVisible(MidReleaseLabel);
	addAndMakeVisible(MidThresholdLabel);
	addAndMakeVisible(MidRatioLabel);


	addAndMakeVisible(HighAttackLabel);
	addAndMakeVisible(HighReleaseLabel);
	addAndMakeVisible(HighThresholdLabel);
	addAndMakeVisible(HighRatioLabel);

}
void ThreeButtonsHigh::buttonClicked(juce::Button* button) {

}

void ThreeButtonsMid::buttonClicked(juce::Button* button) {

}


DistortionComponent::DistortionComponent(juce::AudioProcessorValueTreeState& apv): apvts(apv) {

	addAndMakeVisible(distortionSlider);
	addAndMakeVisible(rangeSlider);
	addAndMakeVisible(mixSlider);
	addAndMakeVisible(gainSlider);
	addAndMakeVisible(offBtn);

	offBtn.setButtonText(juce::String(juce::CharPointer_UTF8("Off")));
	offBtn.setClickingTogglesState(true);
	offBtn.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
	offBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentWhite);
	offBtn.setColour(juce::TextButton::textColourOnId, juce::Colour(110, 164, 38));
	offBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

	distortionSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	rangeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	mixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);

	distortionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	rangeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

	distortionSlider.setNormalisableRange(juce::NormalisableRange<double>(0.0f, 1.0f, 0.0001f, 1.0f));
	rangeSlider.setNormalisableRange(juce::NormalisableRange<double>(0.0f, 150.0f, 1.0f, 1.0f));
	mixSlider.setNormalisableRange(juce::NormalisableRange<double>(0.0f, 1.0f, 0.0001f, 1.0f));
	gainSlider.setNormalisableRange(juce::NormalisableRange<double>(0.0f, 3.0f, 0.0001f, 1.0f));

	distortionSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "drive", distortionSlider);
	rangeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "range", rangeSlider);
	mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "blend", mixSlider);
	gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apv, "volume", gainSlider);
	offBtnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apv, "distortionoff", offBtn);



}
DistortionComponent::~DistortionComponent() {

}

void DistortionComponent::paint(juce::Graphics& g) {

}

void DistortionComponent::resized() {
	auto bounds = getLocalBounds();
	using namespace juce;


	FlexBox flexbox;
	flexbox.flexDirection = FlexBox::Direction::row;
	flexbox.flexWrap = FlexBox::Wrap::noWrap;
	auto spacer = FlexItem().withWidth(8);

	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(distortionSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(rangeSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(mixSlider).withFlex(1.f));
	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(gainSlider).withFlex(1.f));
	flexbox.items.add(spacer);

	spacer = FlexItem().withHeight(20);

	flexbox.items.add(spacer);
	flexbox.items.add(FlexItem(offBtn).withWidth(20).withHeight(15));
	flexbox.items.add(spacer);

	spacer = FlexItem().withWidth(4);

	flexbox.performLayout(bounds);
}

void TheEqualizerAudioProcessorEditor::InitializeGainSliderComponents() {
	InputGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	InputGainSlider.setNormalisableRange(juce::NormalisableRange<double>(-24.f, 24.f, 0.5f, 1.f));
	InputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	inputGainTitle.setText("Input Gain", juce::dontSendNotification);
	inputGainTitle.setFont(juce::Font(14.0f));
	inputGainTitle.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	inputGainTitle.setJustificationType(juce::Justification::centred);

	inputValueRange.setText("-24dB          24dB", juce::dontSendNotification);
	inputValueRange.setFont(juce::Font(14.0f));
	inputValueRange.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	inputValueRange.setJustificationType(juce::Justification::centred);

	OutputGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	OutputGainSlider.setNormalisableRange(juce::NormalisableRange<double>(-24.f, 24.f, 0.5f, 1.f));
	OutputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	outputGainTitle.setText("Makeup Gain", juce::dontSendNotification);
	outputGainTitle.setFont(juce::Font(14.0f));
	outputGainTitle.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	outputGainTitle.setJustificationType(juce::Justification::centred);
	outputValueRange.setText("-24dB          24dB", juce::dontSendNotification);
	outputValueRange.setFont(juce::Font(14.0f));
	outputValueRange.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
	outputValueRange.setJustificationType(juce::Justification::centred);


	ingainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"Mix Gain", InputGainSlider);
	outgainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Makeup Gain", OutputGainSlider);
	addAndMakeVisible(InputGainSlider);
	addAndMakeVisible(OutputGainSlider);
	addAndMakeVisible(inputGainTitle);
	addAndMakeVisible(outputGainTitle);
	addAndMakeVisible(inputValueRange);
	addAndMakeVisible(outputValueRange);
}
//==============================================================================
TheEqualizerAudioProcessorEditor::TheEqualizerAudioProcessorEditor(TheEqualizerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), forwardFFT(p.fftOrder),
	window(p.fftSize, juce::dsp::WindowingFunction<float>::hann)
{

	juce::LookAndFeel::setDefaultLookAndFeel(&clnf);

	
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	

	InitializeLabelsForCompressor();
	InitializeGainSliderComponents();

	addAndMakeVisible(reverbControl);
	addAndMakeVisible(reverbTitles);
	addAndMakeVisible(distortionComponent);
	addAndMakeVisible(distortionTitles);
	addAndMakeVisible(distortionRanges);
	addAndMakeVisible(controller);
	addAndMakeVisible(leftM);
	addAndMakeVisible(rightM);
	addAndMakeVisible(reverbRange);
	addAndMakeVisible(lowBandControls);
	addAndMakeVisible(lowBandTitles);
	addAndMakeVisible(lowBandRanges);
	addAndMakeVisible(threebuttonslow);


	addAndMakeVisible(midBandControls);
	addAndMakeVisible(midBandTitles);
	addAndMakeVisible(midBandRanges);
	addAndMakeVisible(threebuttonsmid);

	addAndMakeVisible(highBandControls);
	addAndMakeVisible(highBandTitles);
	addAndMakeVisible(highBandRanges);
	addAndMakeVisible(threebuttonshigh);

    addAndMakeVisible(title);
    addAndMakeVisible(audioProcessor.wave);


   

    setSize(1150, 650);
	startTimerHz(24);
}
TheEqualizerAudioProcessorEditor::~TheEqualizerAudioProcessorEditor() {
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void TheEqualizerAudioProcessorEditor::timerCallback() {
	auto flg = audioProcessor.getFFTReady();
	if (flg)
	{
		drawNextFrameOfSpectrum();
		audioProcessor.setFFTReady(false);
		repaint();
	}

	leftM.setLevel(audioProcessor.getRMS(0));
	rightM.setLevel(audioProcessor.getRMS(1));

	leftM.repaint();
	rightM.repaint();
}
void TheEqualizerAudioProcessorEditor::drawNextFrameOfSpectrum()
{

}

void TheEqualizerAudioProcessorEditor::drawFrame(juce::Graphics& g)
{
	for (int i = 1; i < scopeSize; ++i)
	{
		auto width = 300;
		auto height = 300;
		
		g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
							  juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
					  (float)juce::jmap(i,     0, scopeSize - 1, 0, width),
							  juce::jmap(scopeData[i],     0.0f, 1.0f, (float)height, 0.0f) });
	}
}


//==============================================================================
void TheEqualizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    back = juce::ImageCache::getFromMemory(BinaryData::The_Equalizertwo_png, BinaryData::The_Equalizertwo_pngSize);
    title.setImage(back, juce::RectanglePlacement::stretchToFit);

    audioProcessor.wave.setColours(juce::Colour(24, 25, 26), juce::Colour(0, 132, 67));


	juce::Rectangle<int> frame(47, 270, 350, 630);
	auto bounds = frame;


	g.setColour(juce::Colour(110, 164, 38));
	g.fillAll();

	auto localbounds = bounds;
	bounds.reduce(3, 3);
	g.setColour(juce::Colour(24, 25, 26));
	g.fillRoundedRectangle(bounds.toFloat(), 3);

	g.drawRect(localbounds);
	g.fillAll(juce::Colour(24, 25, 26));

}

void TheEqualizerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    audioProcessor.wave.setBounds(25, 70, 1095, 50);
    title.setBounds(370, -40, 400, 131);
	leftM.setBounds(1060, 320, 15, 300);
	rightM.setBounds(1080, 320, 15, 300);

	controller.setBounds(25, 270, 400, 360);

	reverbControl.setBounds(450, 290, 400, 70);
	reverbTitles.setBounds(470, 280, 400, 14);
	reverbRange.setBounds(470, 320, 400, 70);

	distortionComponent.setBounds(450, 500, 400, 70);
	distortionTitles.setBounds(470, 490, 400, 15);
	distortionRanges.setBounds(450, 560,400,15);
	/// <summary>
	/// Set up the layout for the Low Band Controls, the titles and their range values
	/// </summary>
	lowBandControls.setBounds(25, 290, 350, 90);
	lowBandTitles.setBounds(47, 280, 350, 14);
	lowBandRanges.setBounds(35, 370, 350, 10);
	threebuttonslow.setBounds(380, 310, 30, 30);
	LowAttackLabel.setBounds(55, 320, 40, 15);
	LowReleaseLabel.setBounds(140, 320, 40, 15);
	LowThresholdLabel.setBounds(225, 320, 40, 15);
	LowRatioLabel.setBounds(310, 320, 40, 15);

	///////////////////////////////////////////////////////////////
	
	midBandControls.setBounds(25, 405, 350, 90);
	midBandTitles.setBounds(47, 395, 350, 14);
	midBandRanges.setBounds(35, 475, 350, 10);
	threebuttonsmid.setBounds(380, 435, 30, 30);
	MidAttackLabel.setBounds(55, 435, 40, 15);
	MidReleaseLabel.setBounds(140, 435, 40, 15);
	MidThresholdLabel.setBounds(225, 435, 40, 15);
	MidRatioLabel.setBounds(310, 435, 40, 15);



	highBandControls.setBounds(25, 520, 350, 90);
	highBandTitles.setBounds(47, 510, 350, 14);
	highBandRanges.setBounds(35, 595, 350, 10);
	threebuttonshigh.setBounds(380, 540, 30, 30);
	HighAttackLabel.setBounds(55, 550, 40, 15);
	HighReleaseLabel.setBounds(140, 550, 40, 15);
	HighThresholdLabel.setBounds(225, 550, 40, 15);
	HighRatioLabel.setBounds(310, 550, 40, 15);


	/// <summary>
	/// Set up the layout for the input gain and output gain sliders, the titles and their range values
	/// </summary>
	/// 
	/// 
	InputGainSlider.setBounds(25, 165, 90, 90);
	OutputGainSlider.setBounds(1040, 165, 90, 90);
	inputGainTitle.setBounds(30, 150, 75, 15);
	outputGainTitle.setBounds(1045, 150, 75, 20);
	inputValueRange.setBounds(25, 245, 90, 15);
	outputValueRange.setBounds(1040, 245, 90, 15);
	///////////////////////////////////////////////////////////////


}
