using Microsoft.UI.Xaml;

using 哈夫曼压缩.Contracts.Services;
using 哈夫曼压缩.ViewModels;

namespace 哈夫曼压缩.Activation;

public class DefaultActivationHandler : ActivationHandler<LaunchActivatedEventArgs>
{
    private readonly INavigationService _navigationService;

    public DefaultActivationHandler(INavigationService navigationService)
    {
        _navigationService = navigationService;
    }

    protected override bool CanHandleInternal(LaunchActivatedEventArgs args)
    {
        // None of the ActivationHandlers has handled the activation.
        return _navigationService.Frame?.Content == null;
    }

    protected async override Task HandleInternalAsync(LaunchActivatedEventArgs args)
    {
        _navigationService.NavigateTo(typeof(压缩ViewModel).FullName!, args.Arguments);

        await Task.CompletedTask;
    }
}
