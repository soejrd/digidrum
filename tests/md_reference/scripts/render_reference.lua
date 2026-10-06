-- Run by pipeline.py in a fresh REAPER instance for each sweep group.
-- The saved project contains the empty E1 pattern and base preset.
local case_file = assert(os.getenv('DD_CASE_FILE'), 'DD_CASE_FILE is required')
local done_file = assert(os.getenv('DD_DONE_FILE'), 'DD_DONE_FILE is required')
local startup_wait_seconds = 20
local project = 0
local track = assert(reaper.GetTrack(project, 0), 'Missing Gearmulator track')
local _, fx_name = reaper.TrackFX_GetFXName(track, 0, '')
assert(fx_name:find('Gearmulator MD'), 'Track 1 FX must be Gearmulator MD')
for i = 0, 7 do
  local _, name = reaper.TrackFX_GetParamName(track, 0, i, '')
  assert(name == 'Track 1 Machine Parameter ' .. (i+1), 'Unexpected parameter ' .. i .. ': ' .. name)
end
local _, script_path = reaper.get_action_context()
local script_dir = script_path:match('^(.*)/[^/]+$')
local output_dir = script_dir .. '/../machines/trx_b2/reference'
reaper.RecursiveCreateDirectory(output_dir, 0)
local cases = {}
for line in io.lines(case_file) do
  local name, params, duration = line:match('^([^\t]+)\t([^\t]+)\t([^\t]+)$')
  assert(name and params and duration, 'Malformed case line')
  local patch = {}
  for value in params:gmatch('[^,]+') do patch[#patch+1] = assert(tonumber(value)) end
  assert(#patch == 8, 'Expected eight parameters')
  cases[#cases+1] = {name=name, patch=patch, duration=assert(tonumber(duration))}
end
assert(#cases > 0, 'No cases to render')

reaper.GetSetProjectInfo(project, 'RENDER_BOUNDSFLAG', 0, true)
reaper.GetSetProjectInfo(project, 'RENDER_STARTPOS', 0, true)
reaper.GetSetProjectInfo(project, 'RENDER_CHANNELS', 1, true)
reaper.GetSetProjectInfo(project, 'RENDER_SRATE', 48000, true)
reaper.GetSetProjectInfo(project, 'RENDER_SETTINGS', 0, true)
reaper.GetSetProjectInfo(project, 'RENDER_NORMALIZE', 0, true)
reaper.GetSetProjectInfo(project, 'RENDER_DITHER', 0, true)
reaper.GetSetProjectInfo_String(project, 'RENDER_FORMAT', 'evaw', true)
reaper.GetSetProjectInfo_String(project, 'RENDER_FILE', output_dir, true)

local function render_case(entry)
  for i = reaper.CountTrackMediaItems(track)-1, 0, -1 do
    reaper.DeleteTrackMediaItem(track, reaper.GetTrackMediaItem(track, i))
  end
  local item = reaper.CreateNewMIDIItemInProj(track, 0, entry.duration, false)
  local take = assert(reaper.GetActiveTake(item))
  local start = reaper.MIDI_GetPPQPosFromProjTime(take, 0.25)
  local finish = reaper.MIDI_GetPPQPosFromProjTime(take, 0.30)
  assert(reaper.MIDI_InsertNote(take, false, false, start, finish, 0, 36, 100, false))
  reaper.MIDI_Sort(take)
  for i = 1, 8 do
    reaper.TrackFX_SetParamNormalized(track, 0, i-1, entry.patch[i]/127)
  end
  reaper.GetSetProjectInfo(project, 'RENDER_ENDPOS', entry.duration, true)
  reaper.GetSetProjectInfo_String(project, 'RENDER_PATTERN', entry.name, true)
  local path = output_dir .. '/' .. entry.name .. '.wav'
  os.remove(path)
  reaper.Main_OnCommand(42230, 0)
  local f = assert(io.open(path, 'rb'), 'Render failed: ' .. path)
  local size = f:seek('end')
  f:close()
  assert(size > 100000, 'Render too small: ' .. path)
  reaper.ShowConsoleMsg('Rendered ' .. entry.name .. '\n')
end

local function run()
  local ok, err = pcall(function()
    -- Gearmulator can keep an internal kit value while Reaper already reports
    -- the target normalized value. Prime with a short, different patch first
    -- so every base-patch SetParamNormalized generates a real change.
    render_case({name='_prime', patch={127,0,0,1,0,1,1,1}, duration=2})
    os.remove(output_dir .. '/_prime.wav')
    for _, entry in ipairs(cases) do render_case(entry) end
  end)
  local f = assert(io.open(done_file, 'w'))
  f:write(ok and 'ok' or tostring(err))
  f:close()
end
local ready_at = reaper.time_precise() + startup_wait_seconds
local function wait_for_boot()
  if reaper.time_precise() < ready_at then
    reaper.defer(wait_for_boot)
  else
    run()
  end
end
wait_for_boot()
