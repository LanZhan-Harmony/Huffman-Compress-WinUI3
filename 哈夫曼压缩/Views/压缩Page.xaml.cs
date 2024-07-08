using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using 哈夫曼压缩.ViewModels;
using Windows.Storage.Pickers;
using Windows.Storage;
using Windows.Storage.AccessCache;
using 哈夫曼压缩.CoreEngine;
namespace 哈夫曼压缩.Views;
public sealed partial class 压缩Page : Page
{
    private StorageFile file;
    private StorageFolder folder;
    public 压缩ViewModel ViewModel
    {
        get;
    }

    public 压缩Page()
    {
        ViewModel = App.GetService<压缩ViewModel>();
        InitializeComponent();
    }
    private async void PickAFileButton_Click(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        PickAFileOutputTextBlock.Text = "";

        var openPicker = new Windows.Storage.Pickers.FileOpenPicker();

        // See the sample code below for how to make the window accessible from the App class.
        var window = App.MainWindow;

        // Retrieve the window handle (HWND) of the current WinUI 3 window.
        var hWnd = WinRT.Interop.WindowNative.GetWindowHandle(window);

        // Initialize the file picker with the window handle (HWND).
        WinRT.Interop.InitializeWithWindow.Initialize(openPicker, hWnd);

        // Set options for your file picker
        openPicker.ViewMode = Windows.Storage.Pickers.PickerViewMode.Thumbnail;
        openPicker.FileTypeFilter.Add("*");

        // Open the picker for the user to pick a file
        file = await openPicker.PickSingleFileAsync();
        if (file != null)
        {
            PickAFileOutputTextBlock.Text = "已选择文件: " + file.Name;
        }
        else
        {
            PickAFileOutputTextBlock.Text = "已取消选择。";
        }
        if (folder != null && file != null)
        {
            StartCompress.IsEnabled = true;
        }
    }

    private async void PickFolderButton_Click(object sender, RoutedEventArgs e)
    {
        // Clear previous returned file name, if it exists, between iterations of this scenario
        PickFolderOutputTextBlock.Text = "";

        // Create a folder picker
        FolderPicker openPicker = new Windows.Storage.Pickers.FolderPicker();

        // See the sample code below for how to make the window accessible from the App class.
        var window = App.MainWindow;

        // Retrieve the window handle (HWND) of the current WinUI 3 window.
        var hWnd = WinRT.Interop.WindowNative.GetWindowHandle(window);

        // Initialize the folder picker with the window handle (HWND).
        WinRT.Interop.InitializeWithWindow.Initialize(openPicker, hWnd);

        // Set options for your folder picker
        openPicker.SuggestedStartLocation = PickerLocationId.Desktop;
        openPicker.FileTypeFilter.Add("*");

        // Open the picker for the user to pick a folder
        folder = await openPicker.PickSingleFolderAsync();
        if (folder != null)
        {
            StorageApplicationPermissions.FutureAccessList.AddOrReplace("PickedFolderToken", folder);
            PickFolderOutputTextBlock.Text = "已选择文件夹: " + folder.Name;
        }
        else
        {
            PickFolderOutputTextBlock.Text = "已取消选择";
        }
        if (folder != null && file != null)
        {
            StartCompress.IsEnabled = true;
        }
    }

    private void StartCompress_Click(object sender, RoutedEventArgs e)
    {
        var filename = file.Path;
        var foldername = folder.Path;
        // 按了压缩按钮后文字改变
        StartCompress.Content = "正在压缩...";
        // 压缩文件
        var compress = new Compression();
        try
        {
            var compressibility = compress.Compress(filename, foldername);
            StartCompress.Content = "压缩完成";
            CompressibilityTextBlock.Text = "压缩率: " + (compressibility/100).ToString("P2");
            Refresh.IsEnabled = true;
        }
        catch (Exception)
        {
            StartCompress.Content = "压缩失败";
            Refresh.IsEnabled = true;
        }
    }

    private void Refresh_Click(object sender, RoutedEventArgs e)
    {
        Frame.Navigate(typeof(压缩Page));
    }
}
