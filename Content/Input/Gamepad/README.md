# Configuración de Input para Gamepad

Este documento explica cómo configurar el soporte para gamepad en el proyecto Dungeon Crawler.

## Assets necesarios a crear:

### 1. Input Actions (en Content/Input/Gamepad/)

Crear los siguientes Input Actions:

#### IA_Move (Input Action)
- **Value Type**: Axis2D
- **Triggers**: 
  - Default (Chorded Action con modificadores)

#### IA_Look (Input Action)
- **Value Type**: Axis2D
- **Triggers**:
  - Default (Chorded Action con modificadores)

#### IA_Jump (Input Action)
- **Value Type**: Boolean
- **Triggers**:
  - Pressed
  - Released

#### IA_Run (Input Action)
- **Value Type**: Boolean
- **Triggers**:
  - Pressed
  - Released

#### IA_Crouch (Input Action)
- **Value Type**: Boolean
- **Triggers**:
  - Pressed
  - Released

#### IA_Dash (Input Action)
- **Value Type**: Boolean
- **Triggers**:
  - Pressed

#### IA_Inventory (Input Action)
- **Value Type**: Boolean
- **Triggers**:
  - Pressed

#### IA_Pickup (Input Action)
- **Value Type**: Boolean
- **Triggers**:
  - Pressed

#### IA_Drop (Input Action)
- **Value Type**: Boolean
- **Triggers**:
  - Pressed

### 2. Input Mapping Context (en Content/Input/)

#### IMC_Gamepad (Input Mapping Context)

Agregar los siguientes mappings:

#### Mappings para Teclado y Ratón (Keyboard & Mouse):
- **IA_Move**:
  - W/S (Move Forward/Backward)
  - A/D (Move Left/Right)
- **IA_Look**:
  - Mouse X/Y (Look)
- **IA_Jump**: Space
- **IA_Run**: Left Shift
- **IA_Crouch**: Left Ctrl
- **IA_Dash**: Space (mientras corres)
- **IA_Inventory**: I o Tab
- **IA_Pickup**: V
- **IA_Drop**: R

#### Mappings para Gamepad (Gamepad):
- **IA_Move**:
  - Left Stick X/Y
- **IA_Look**:
  - Right Stick X/Y
- **IA_Jump**: A Button (South Face Button)
- **IA_Run**: Left Trigger o Left Shoulder
- **IA_Crouch**: B Button (East Face Button) o Right Stick Click
- **IA_Dash**: X Button (West Face Button)
- **IA_Inventory**: Y Button (North Face Button) o Start
- **IA_Pickup**: A Button (South Face Button) - mantener presionado cerca de items
- **IA_Drop**: B Button (East Face Button) - mantener presionado

## Configuración en el Character:

En el Blueprint del MyPlayerCharacter o en el código:

1. Asignar el **IMC_Gamepad** al campo **Input Mapping Context**
2. Asignar cada **Input Action** a su correspondiente campo:
   - Move Action → IA_Move
   - Look Action → IA_Look
   - Jump Action → IA_Jump
   - Run Action → IA_Run
   - Crouch Action → IA_Crouch
   - Dash Action → IA_Dash
   - Inventory Action → IA_Inventory
   - Pickup Action → IA_Pickup
   - Drop Action → IA_Drop

## Notas importantes:

- El sistema mantiene compatibilidad con el input legacy como fallback
- Los mappings de gamepad están optimizados para Xbox-style controllers
- Se puede ajustar la sensibilidad del gamepad en los Input Actions
- El sistema Enhanced Input permite modificar los controles en runtime