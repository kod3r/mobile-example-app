﻿// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

using System;
using System.Windows;
using System.Windows.Media;
using ExampleApp;
using System.Windows.Media.Animation;

namespace ExampleAppWPF
{
    public class FlattenButtonView : ToggleButtonBase
    {
        private double m_yPosActive;
        private double m_yPosInactive;
        private IntPtr m_nativeCallerPointer;
        private double m_stateChangeAnimationTimeMilliseconds = 200;
        private bool m_isFirstLayout = true;

        static FlattenButtonView()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(FlattenButtonView), new FrameworkPropertyMetadata(typeof(FlattenButtonView)));
        }

        public FlattenButtonView(IntPtr nativeCallerPointer)
        {
            m_nativeCallerPointer = nativeCallerPointer;
            Click += FlattenButton_Click;
            Loaded += PerformLayout;

            MainWindow mainWindow = (MainWindow)Application.Current.MainWindow;
            mainWindow.SizeChanged += PerformLayout;
            mainWindow.MainGrid.Children.Add(this);
        }

        private void PerformLayout(object sender, RoutedEventArgs e)
        {
            Point currentPosition = RenderTransform.Transform(new Point(0.0, 0.0));
            double onScreenState = (currentPosition.Y - m_yPosInactive) / (m_yPosActive - m_yPosInactive);

            MainWindow mainWindow = (MainWindow)Application.Current.MainWindow;
            double screenHeight = mainWindow.ActualHeight;
            double screenWidth = mainWindow.ActualWidth;
            double viewHeight = ActualHeight;
            double viewWidth = ActualWidth;
            m_yPosActive = screenHeight * 0.5 - (viewHeight * 0.5 + ConversionHelpers.AndroidToWindowsDip(16));
            m_yPosInactive = screenHeight * 0.5 + viewHeight * 0.5;
            double layoutY = m_yPosInactive;

            if (!m_isFirstLayout)
            {
                layoutY = onScreenState * (m_yPosActive - m_yPosInactive) + m_yPosInactive;
            }

            m_isFirstLayout = false;

            var transform = new TranslateTransform(
                -(viewWidth * 0.5 + ConversionHelpers.AndroidToWindowsDip(48)),
                layoutY);

            RenderTransform = transform;
        }
        public void Destroy()
        {
            MainWindow mainWindow = (MainWindow)Application.Current.MainWindow;
            mainWindow.SizeChanged -= PerformLayout;
            mainWindow.MainGrid.Children.Remove(this);
        }

        public void UpdateViewStateBasedOnFlattening(bool isFlattened)
        {
            IsChecked = isFlattened;
        }

        public void AnimateToInactive()
        {
            AnimateViewToY(m_yPosInactive);
        }

        public void AnimateToActive()
        {
            AnimateViewToY(m_yPosActive);
        }

        public void AnimateViewToY(double y)
        {
            var currentPosition = RenderTransform.Transform(new Point(0.0, 0.0));
            var animation = new DoubleAnimation();
            animation.From = currentPosition.Y;
            animation.To = y;
            animation.Duration = new Duration(TimeSpan.FromMilliseconds(m_stateChangeAnimationTimeMilliseconds));
            animation.EasingFunction = new SineEase();

            var transform = new TranslateTransform(currentPosition.X, currentPosition.Y);
            RenderTransform = transform;
            transform.BeginAnimation(TranslateTransform.YProperty, animation);
        }

        public void AnimateToIntermediateOnScreenState(float onScreenState)
        {
            Point currentPosition = RenderTransform.Transform(new Point(0.0, 0.0));
            double viewY = currentPosition.Y;
            double newY = m_yPosInactive + (m_yPosActive - m_yPosInactive) * onScreenState;

            if (viewY != newY)
            {
                RenderTransform = new TranslateTransform(currentPosition.X, newY);
            }
        }

        private void FlattenButton_Click(object sender, RoutedEventArgs e)
        {
            FlattenButtonViewCLIMethods.OnToggle(m_nativeCallerPointer, IsChecked.Value);
        }
    }
}
