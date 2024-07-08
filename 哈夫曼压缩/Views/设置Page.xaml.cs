using Microsoft.UI.Xaml.Controls;

using 哈夫曼压缩.ViewModels;

namespace 哈夫曼压缩.Views;

// TODO: Set the URL for your privacy policy by updating 设置Page_PrivacyTermsLink.NavigateUri in Resources.resw.
public sealed partial class 设置Page : Page
{
    public 设置ViewModel ViewModel
    {
        get;
    }

    public 设置Page()
    {
        ViewModel = App.GetService<设置ViewModel>();
        InitializeComponent();
    }
}
