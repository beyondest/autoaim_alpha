from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess, RegisterEventHandler,EmitEvent
from launch.event_handlers import OnProcessExit
package_name = 'autoaim_alpha'
def generate_launch_description():
    return LaunchDescription([
        Node(
            package='autoaim_alpha',
            node_executable='node_test2',
            node_name='node_test2',
            output='screen'
        ),
        RegisterEventHandler(
            event_handler=OnProcessExit(
                target_action=Node(
                    package='autoaim_alpha',
                    node_executable='node_test2',
                    node_name='node_test2',
                    output='screen'
                ),
                on_exit=EmitEvent(
                    event=ExecuteProcess(
                        cmd=['ros2', 'run', 'autoaim_alpha', 'node_test2'],
                        output='screen',
                        additional_env={'RCUTILS_CONSOLE_STDOUT_LINE_BUFFERED': '1'},
                        name='node_test2'
                    )
                )
            )
        )
    ])