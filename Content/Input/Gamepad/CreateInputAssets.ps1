# Script para crear assets de input para gamepad
# Ejecutar desde el directorio del proyecto

Write-Host "Creando assets de input para gamepad..."

# Crear Input Actions
$inputActions = @(
    "IA_Move",
    "IA_Look",
    "IA_Jump",
    "IA_Run",
    "IA_Crouch",
    "IA_Dash",
    "IA_Inventory",
    "IA_Pickup",
    "IA_Drop"
)

foreach ($action in $inputActions) {
    $path = "Content/Input/Gamepad/$action.uasset"
    Write-Host "Creando: $path"
    # Nota: Este script solo muestra qué archivos crear.
    # Los assets deben crearse manualmente en el editor de Unreal Engine.
}

Write-Host "Assets a crear manualmente en Unreal Engine:"
Write-Host "1. Input Mapping Context: IMC_Gamepad"
Write-Host "2. Input Actions: $($inputActions -join ', ')"
Write-Host ""
Write-Host "Ver README.md para instrucciones detalladas de configuración."