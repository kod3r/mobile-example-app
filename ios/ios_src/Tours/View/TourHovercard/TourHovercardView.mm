// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "TourHovercardView.h"
#include <algorithm>
#include "MathFunc.h"
#include "UIColors.h"
#include "ImageHelpers.h"
#include "WorldPinOnMapViewInterop.h"
#include "App.h"
#include "Document.h"
#include "ImageStore.h"
#include "TourDefines.h"

@implementation TourHovercardView
{
    ExampleApp::WorldPins::View::WorldPinOnMapViewInterop* m_pInterop;
    UITapGestureRecognizer* m_tapGestureRecogniser;
    float m_stateChangeAnimationTimeSeconds;
    float m_pinOffset;
    float m_pixelScale;
    
    float m_cardHeight;
    float m_labelHeight;
    
    float m_previousX;
    float m_previousY;
    
    std::string m_imagePath;
    bool m_isVideo;
    ImageStore* m_pImageStore;
    
    ExampleApp::Helpers::ColorHelpers::Color m_baseColor;
    ExampleApp::Helpers::ColorHelpers::Color m_textColor;
}
- (id)initWithParams:(float)pinDiameter :(float)pixelScale :(ImageStore*)pImageStore :(ExampleApp::WorldPins::View::WorldPinOnMapViewInterop*)interop
{
    self = [super init];
    
    if(self)
    {
        m_pImageStore = pImageStore;
        
        self.alpha = 0.f;
        m_pinOffset = (pinDiameter * pixelScale);
        m_pixelScale = pixelScale;
        m_stateChangeAnimationTimeSeconds = 0.2f;
        
        m_tapGestureRecogniser = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTapped:)];
        [m_tapGestureRecogniser setDelegate:self];
        [self addGestureRecognizer: m_tapGestureRecogniser];
        
        m_pInterop = interop;
        
        // shadow
        self.pMainControlShadowContainer = [[[UIView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)] autorelease];
        [self.pMainControlShadowContainer setBackgroundColor: ExampleApp::Helpers::ColorPalette::GreyTone];
        [self addSubview: self.pMainControlShadowContainer];
        
        // main control container
        self.pMainControlContainer = [[[UIView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)] autorelease];
        [self.pMainControlContainer setBackgroundColor: [UIColor clearColor]];
        [self addSubview: self.pMainControlContainer];
        
        // gold top strip
        self.pTopStrip = [[[UIView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)] autorelease];
        [self.pTopStrip setBackgroundColor: [UIColor colorWithRed:m_baseColor.GetRedF() green:m_baseColor.GetGreenF() blue:m_baseColor.GetBlueF() alpha:1.0f]];
        [self.pMainControlContainer addSubview: self.pTopStrip];
        
        // label container
        self.pLabelBack = [[[UIView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)] autorelease];
        [self.pLabelBack setBackgroundColor: ExampleApp::Helpers::ColorPalette::MainHudColor];
        [self.pMainControlContainer addSubview: self.pLabelBack];
        
        // name label
        const float nameFontSize = 16.f;
        self.pNameLabel = [[[UILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)] autorelease];
        self.pNameLabel.textColor = [UIColor colorWithRed:m_textColor.GetRedF() green:m_textColor.GetGreenF() blue:m_textColor.GetBlueF() alpha:1.0f];
        self.pNameLabel.textAlignment = NSTextAlignmentCenter;
        self.pNameLabel.font = [UIFont systemFontOfSize:nameFontSize];
        self.pNameLabel.backgroundColor = [UIColor clearColor];
        [self.pLabelBack addSubview: self.pNameLabel];
        
        // info label
        const float infoFontSize = 12.f;
        self.pInfoLabel = [[[UILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)] autorelease];
        self.pInfoLabel.textColor = TourDefines::LightTextColor;
        self.pInfoLabel.textAlignment = NSTextAlignmentCenter;
        self.pInfoLabel.font = [UIFont systemFontOfSize:infoFontSize];
        self.pInfoLabel.backgroundColor = [UIColor clearColor];
        [self.pLabelBack addSubview: self.pInfoLabel];
        
        // poi arrow
        self.pArrowContainer = [[[UIImageView alloc] initWithImage:ExampleApp::Helpers::ImageHelpers::LoadImage("arrow1")] autorelease];
        self.pArrowContainer.contentMode = UIViewContentModeScaleToFill;
        [self addSubview: self.pArrowContainer];
        
        self.frame.origin = CGPointMake(0, 0);
    }
    
    return self;
}

- (void)dealloc
{
    [self.pNameLabel removeFromSuperview];
    [self.pNameLabel release];
    
    [self.pInfoLabel removeFromSuperview];
    [self.pInfoLabel release];
    
    [self.pLabelBack removeFromSuperview];
    [self.pLabelBack release];
    
    [self.pTopStrip removeFromSuperview];
    [self.pTopStrip release];
    
    [self.pMainControlContainer removeFromSuperview];
    [self.pMainControlContainer release];
    
    [self.pMainControlShadowContainer removeFromSuperview];
    [self.pMainControlShadowContainer release];
    
    [self.pArrowContainer removeFromSuperview];
    [self.pArrowContainer release];
    
    [self removeFromSuperview];
    [super dealloc];
}

- (void)layoutSubviews
{
    
    self.layer.shouldRasterize = YES;
    self.layer.rasterizationScale = [[UIScreen mainScreen] scale];
    
    // figures from a proportional size of an iPad2 screen -- use const so not proportional to small screens on iPhones.
    const float w = 162.f;
    const float h = 56.f;
    const float labelContainerOffsetY = 6.f;
    const float labelVerticalSpace = h*0.4f;
    const float labelSpacing = h*0.05f;
    const float labelOffsetX = 4.f;
    const float labelOffsetY = 2.f;
    
    
    self.pMainControlContainer.frame = CGRectMake(0, 0, w, h);
    self.pMainControlShadowContainer.frame = CGRectMake(2.f, 2.f, w, h);
    
    
    const CGFloat labelContainerHeight = self.pMainControlContainer.frame.size.height - labelContainerOffsetY;
    
    self.pTopStrip.frame =  CGRectMake(0.f, 0.f, w, labelContainerOffsetY);
    self.pLabelBack.frame = CGRectMake(0.f, labelContainerOffsetY, w, labelContainerHeight);
    
    self.pNameLabel.frame = CGRectMake(labelOffsetX,
                                       labelOffsetY,
                                       w - (labelOffsetX*2),
                                       labelVerticalSpace);
    
    self.pInfoLabel.frame = CGRectMake(labelOffsetX,
                                          labelVerticalSpace + labelSpacing,
                                          w - (labelOffsetX*2),
                                          labelVerticalSpace);
    
    
    const float arrowWidth = 16.f;
    self.pArrowContainer.frame = CGRectMake(w/2.f - arrowWidth/2.f, h, arrowWidth, arrowWidth);
    
    float trueY = m_previousY/m_pixelScale - m_pinOffset/m_pixelScale;
    float trueX = m_previousX/m_pixelScale;
    self.frame = CGRectMake(trueX - w/2, trueY - (h + arrowWidth), w, h + arrowWidth);
    
    m_cardHeight = h + arrowWidth;
}

- (void) setContent:(const ExampleApp::WorldPins::SdkModel::IWorldPinsInFocusModel&) worldPinsInFocusModel
{
    self.pNameLabel.text = [NSString stringWithUTF8String:worldPinsInFocusModel.GetTitle().c_str()];
    self.pInfoLabel.text = [NSString stringWithUTF8String:worldPinsInFocusModel.GetSubtitle().c_str()];
    
    rapidjson::Document jsonData;
    jsonData.Parse<0>(worldPinsInFocusModel.GetJsonData().c_str());
    if(!jsonData.HasParseError())
    {
        Eegeo_ASSERT(jsonData.HasMember("base_col") &&
                     jsonData.HasMember("text_col"));
        
        Byte baseColR = static_cast<Byte>(jsonData["base_col"]["r"].GetInt());
        Byte baseColG = static_cast<Byte>(jsonData["base_col"]["g"].GetInt());
        Byte baseColB = static_cast<Byte>(jsonData["base_col"]["b"].GetInt());
        
        Byte textColR = static_cast<Byte>(jsonData["text_col"]["r"].GetInt());
        Byte textColG = static_cast<Byte>(jsonData["text_col"]["g"].GetInt());
        Byte textColB = static_cast<Byte>(jsonData["text_col"]["b"].GetInt());
        
        [self setPresentationColors :ExampleApp::Helpers::ColorHelpers::Color::FromRGB(baseColR, baseColG, baseColB)
                              :ExampleApp::Helpers::ColorHelpers::Color::FromRGB(textColR, textColG, textColB)];
    }
    
    [self setNeedsLayout];
}

- (void) setFullyActive :(float)modality
{
    self.userInteractionEnabled = YES;
    [self animateToAlpha:(1.f - modality)];
}

- (void) setFullyInactive
{
    self.userInteractionEnabled = NO;
    [self animateToAlpha:0.f];
}

- (void) updatePosition:(float)x :(float)y
{
    float roundedX = roundf(x);
    float roundedY = roundf(y);
    
    if(m_previousX == roundedX && m_previousY == roundedY)
    {
        float trueY = roundedY/m_pixelScale - m_pinOffset/m_pixelScale;
        float trueX = roundedX/m_pixelScale;
        
        CGRect f = self.frame;
        f.origin.x = static_cast<int>(trueX - (f.size.width/2.f));
        f.origin.y = static_cast<int>(trueY - (m_cardHeight));
        self.frame = f;
    }
    else
    {
        float trueY = y/m_pixelScale - m_pinOffset/m_pixelScale;
        float trueX = x/m_pixelScale;
        
        CGRect f = self.frame;
        f.origin.x = trueX - (f.size.width/2.f);
        f.origin.y = trueY - (m_cardHeight);
        self.frame = f;
    }
    
    m_previousX = roundedX;
    m_previousY = roundedY;
}


- (void) setPresentationColors :(ExampleApp::Helpers::ColorHelpers::Color)baseColor :(ExampleApp::Helpers::ColorHelpers::Color)textColor
{
    m_baseColor = baseColor;
    m_textColor = textColor;
    
    [self.pTopStrip setBackgroundColor: [UIColor colorWithRed:m_baseColor.GetRedF() green:m_baseColor.GetGreenF() blue:m_baseColor.GetBlueF() alpha:1.0f]];
    self.pNameLabel.textColor = [UIColor colorWithRed:m_textColor.GetRedF() green:m_textColor.GetGreenF() blue:m_textColor.GetBlueF() alpha:1.0f];
}

- (BOOL)consumesTouch:(UITouch *)touch
{
    if(self.userInteractionEnabled)
    {
        CGPoint touchLocation = [touch locationInView:self];
        return CGRectContainsPoint(self.bounds, touchLocation);
    }
    
    return false;
}

- (void) animateToAlpha:(float)alpha
{
    [UIView animateWithDuration:m_stateChangeAnimationTimeSeconds
                     animations:^
     {
         self.alpha = alpha;
     }];
}

- (void) onTapped:(UITapGestureRecognizer *)recognizer
{
    m_pInterop->OnSelected();
}

@end

